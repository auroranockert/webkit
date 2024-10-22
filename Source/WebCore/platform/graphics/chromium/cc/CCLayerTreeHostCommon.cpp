/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "config.h"

#include "cc/CCLayerTreeHostCommon.h"

#include "FloatQuad.h"
#include "IntRect.h"
#include "LayerChromium.h"
#include "RenderSurfaceChromium.h"
#include "cc/CCLayerImpl.h"
#include "cc/CCLayerIterator.h"
#include "cc/CCLayerSorter.h"
#include "cc/CCMathUtil.h"
#include "cc/CCRenderSurface.h"
#include <public/WebTransformationMatrix.h>

using WebKit::WebTransformationMatrix;

namespace WebCore {

IntRect CCLayerTreeHostCommon::calculateVisibleRect(const IntRect& targetSurfaceRect, const IntRect& layerBoundRect, const WebTransformationMatrix& transform)
{
    // Is this layer fully contained within the target surface?
    IntRect layerInSurfaceSpace = CCMathUtil::mapClippedRect(transform, layerBoundRect);
    if (targetSurfaceRect.contains(layerInSurfaceSpace))
        return layerBoundRect;

    // If the layer doesn't fill up the entire surface, then find the part of
    // the surface rect where the layer could be visible. This avoids trying to
    // project surface rect points that are behind the projection point.
    IntRect minimalSurfaceRect = targetSurfaceRect;
    minimalSurfaceRect.intersect(layerInSurfaceSpace);

    // Project the corners of the target surface rect into the layer space.
    // This bounding rectangle may be larger than it needs to be (being
    // axis-aligned), but is a reasonable filter on the space to consider.
    // Non-invertible transforms will create an empty rect here.
    const WebTransformationMatrix surfaceToLayer = transform.inverse();
    IntRect layerRect = enclosingIntRect(CCMathUtil::projectClippedRect(surfaceToLayer, FloatRect(minimalSurfaceRect)));
    layerRect.intersect(layerBoundRect);
    return layerRect;
}

template<typename LayerType, typename RenderSurfaceType>
static IntRect calculateLayerScissorRect(LayerType* layer, const FloatRect& rootScissorRect)
{
    RenderSurfaceType* targetSurface = layer->targetRenderSurface();

    FloatRect rootScissorRectInTargetSurface = targetSurface->computeRootScissorRectInCurrentSurface(rootScissorRect);
    FloatRect clipAndDamage;
    if (layer->usesLayerClipping())
        clipAndDamage = intersection(rootScissorRectInTargetSurface, layer->clipRect());
    else
        clipAndDamage = intersection(rootScissorRectInTargetSurface, targetSurface->contentRect());

    return enclosingIntRect(clipAndDamage);
}

template<typename LayerType, typename RenderSurfaceType>
static IntRect calculateSurfaceScissorRect(LayerType* layer, const FloatRect& rootScissorRect)
{
    LayerType* parentLayer = layer->parent();
    RenderSurfaceType* targetSurface = parentLayer->targetRenderSurface();
    ASSERT(targetSurface);

    RenderSurfaceType* currentSurface = layer->renderSurface();
    ASSERT(currentSurface);

    FloatRect clipRect = currentSurface->clipRect();

    // For surfaces, empty clipRect means the same as CCLayerImpl::usesLayerClipping being false
    if (clipRect.isEmpty())
        clipRect = intersection(targetSurface->contentRect(), currentSurface->drawableContentRect());

    FloatRect rootScissorRectInTargetSurface = targetSurface->computeRootScissorRectInCurrentSurface(rootScissorRect);

    FloatRect clipAndDamage = intersection(rootScissorRectInTargetSurface, clipRect);
    return enclosingIntRect(clipAndDamage);
}

template<typename LayerType>
static inline bool layerIsInExisting3DRenderingContext(LayerType* layer)
{
    // According to current W3C spec on CSS transforms, a layer is part of an established
    // 3d rendering context if its parent has transform-style of preserves-3d.
    return layer->parent() && layer->parent()->preserves3D();
}

template<typename LayerType>
static bool layerIsRootOfNewRenderingContext(LayerType* layer)
{
    // According to current W3C spec on CSS transforms (Section 6.1), a layer is the
    // beginning of 3d rendering context if its parent does not have transform-style:
    // preserve-3d, but this layer itself does.
    if (layer->parent())
        return !layer->parent()->preserves3D() && layer->preserves3D();

    return layer->preserves3D();
}

template<typename LayerType>
static bool isLayerBackFaceVisible(LayerType* layer)
{
    // The current W3C spec on CSS transforms says that backface visibility should be
    // determined differently depending on whether the layer is in a "3d rendering
    // context" or not. For Chromium code, we can determine whether we are in a 3d
    // rendering context by checking if the parent preserves 3d.

    if (layerIsInExisting3DRenderingContext(layer))
        return layer->drawTransform().isBackFaceVisible();

    // In this case, either the layer establishes a new 3d rendering context, or is not in
    // a 3d rendering context at all.
    return layer->transform().isBackFaceVisible();
}

template<typename LayerType>
static bool isSurfaceBackFaceVisible(LayerType* layer, const WebTransformationMatrix& drawTransform)
{
    if (layerIsInExisting3DRenderingContext(layer))
        return drawTransform.isBackFaceVisible();

    if (layerIsRootOfNewRenderingContext(layer))
        return layer->transform().isBackFaceVisible();

    // If the renderSurface is not part of a new or existing rendering context, then the
    // layers that contribute to this surface will decide back-face visibility for themselves.
    return false;
}

template<typename LayerType>
static IntRect calculateVisibleLayerRect(LayerType* layer)
{
    ASSERT(layer->targetRenderSurface());

    IntRect targetSurfaceRect = layer->targetRenderSurface()->contentRect();

    if (layer->usesLayerClipping())
        targetSurfaceRect.intersect(layer->clipRect());

    if (targetSurfaceRect.isEmpty() || layer->contentBounds().isEmpty())
        return IntRect();

    // Note carefully these are aliases
    const IntSize& bounds = layer->bounds();
    const IntSize& contentBounds = layer->contentBounds();

    const IntRect layerBoundRect = IntRect(IntPoint(), contentBounds);
    WebTransformationMatrix transform = layer->drawTransform();

    transform.scaleNonUniform(bounds.width() / static_cast<double>(contentBounds.width()),
                              bounds.height() / static_cast<double>(contentBounds.height()));
    transform.translate(-contentBounds.width() / 2.0, -contentBounds.height() / 2.0);

    IntRect visibleLayerRect = CCLayerTreeHostCommon::calculateVisibleRect(targetSurfaceRect, layerBoundRect, transform);
    return visibleLayerRect;
}

static bool isScaleOrTranslation(const WebTransformationMatrix& m)
{
    return !m.m12() && !m.m13() && !m.m14()
           && !m.m21() && !m.m23() && !m.m24()
           && !m.m31() && !m.m32() && !m.m43()
           && m.m44();
}

static inline bool layerOpacityIsOpaque(CCLayerImpl* layer)
{
    return layer->opacity() == 1;
}

static inline bool layerOpacityIsOpaque(LayerChromium* layer)
{
    // If the opacity is being animated then the opacity on the main thread is unreliable
    // (since the impl thread may be using a different opacity), so it should not be trusted.
    // In particular, it should not be treated as opaque.
    return layer->opacity() == 1 && !layer->opacityIsAnimating();
}

static inline bool transformToParentIsKnown(CCLayerImpl*)
{
    return true;
}

static inline bool transformToParentIsKnown(LayerChromium* layer)
{
    return !layer->transformIsAnimating();
}

static inline bool transformToScreenIsKnown(CCLayerImpl*)
{
    return true;
}

static inline bool transformToScreenIsKnown(LayerChromium* layer)
{
    return !layer->screenSpaceTransformIsAnimating();
}

template<typename LayerType>
static bool layerShouldBeSkipped(LayerType* layer)
{
    // Layers can be skipped if any of these conditions are met.
    //   - does not draw content.
    //   - is transparent
    //   - has empty bounds
    //   - the layer is not double-sided, but its back face is visible.
    //
    // Some additional conditions need to be computed at a later point after the recursion is finished.
    //   - the intersection of render surface content and layer clipRect is empty
    //   - the visibleLayerRect is empty
    //
    // Note, if the layer should not have been drawn due to being fully transparent,
    // we would have skipped the entire subtree and never made it into this function,
    // so it is safe to omit this check here.

    if (!layer->drawsContent() || layer->bounds().isEmpty())
        return true;

    LayerType* backfaceTestLayer = layer;
    if (layer->useParentBackfaceVisibility()) {
        ASSERT(layer->parent());
        ASSERT(!layer->parent()->useParentBackfaceVisibility());
        backfaceTestLayer = layer->parent();
    }

    // The layer should not be drawn if (1) it is not double-sided and (2) the back of the layer is known to be facing the screen.
    if (!backfaceTestLayer->doubleSided() && transformToScreenIsKnown(backfaceTestLayer) && isLayerBackFaceVisible(backfaceTestLayer))
        return true;

    return false;
}

static inline bool subtreeShouldBeSkipped(CCLayerImpl* layer)
{
    // The opacity of a layer always applies to its children (either implicitly
    // via a render surface or explicitly if the parent preserves 3D), so the
    // entire subtree can be skipped if this layer is fully transparent.
    return !layer->opacity();
}

static inline bool subtreeShouldBeSkipped(LayerChromium* layer)
{
    // If the opacity is being animated then the opacity on the main thread is unreliable
    // (since the impl thread may be using a different opacity), so it should not be trusted.
    // In particular, it should not cause the subtree to be skipped.
    return !layer->opacity() && !layer->opacityIsAnimating();
}

template<typename LayerType>
static bool subtreeShouldRenderToSeparateSurface(LayerType* layer, bool axisAlignedWithRespectToParent)
{
    // Cache this value, because otherwise it walks the entire subtree several times.
    bool descendantDrawsContent = layer->descendantDrawsContent();

    //
    // A layer and its descendants should render onto a new RenderSurface if any of these rules hold:
    //

    // If we force it.
    if (layer->forceRenderSurface())
        return true;

    // If the layer uses a mask.
    if (layer->maskLayer())
        return true;

    // If the layer has a reflection.
    if (layer->replicaLayer())
        return true;

    // If the layer uses a CSS filter.
    if (!layer->filters().isEmpty() || !layer->backgroundFilters().isEmpty())
        return true;

    // If the layer flattens its subtree (i.e. the layer doesn't preserve-3d), but it is
    // treated as a 3D object by its parent (i.e. parent does preserve-3d).
    if (layerIsInExisting3DRenderingContext(layer) && !layer->preserves3D() && descendantDrawsContent)
        return true;

    // If the layer clips its descendants but it is not axis-aligned with respect to its parent.
    // On the main thread, when the transform is being animated, it is treated as unknown and we
    // always error on the side of making a render surface, to let us consider descendents as
    // not animating relative to their target to aid culling.
    if (layer->masksToBounds() && (!axisAlignedWithRespectToParent || !transformToParentIsKnown(layer)) && descendantDrawsContent)
        return true;

    // If the layer has opacity != 1 and does not have a preserves-3d transform style.
    // On the main thread, when opacity is being animated, it is treated as neither 1
    // nor 0.
    if (!layerOpacityIsOpaque(layer) && !layer->preserves3D() && descendantDrawsContent)
        return true;

    return false;
}

WebTransformationMatrix computeScrollCompensationForThisLayer(CCLayerImpl* scrollingLayer, const WebTransformationMatrix& parentMatrix)
{
    // For every layer that has non-zero scrollDelta, we have to compute a transform that can undo the
    // scrollDelta translation. In particular, we want this matrix to premultiply a fixed-position layer's
    // parentMatrix, so we design this transform in three steps as follows. The steps described here apply
    // from right-to-left, so Step 1 would be the right-most matrix:
    //
    //     Step 1. transform from target surface space to the exact space where scrollDelta is actually applied.
    //           -- this is inverse of the matrix in step 3
    //     Step 2. undo the scrollDelta
    //           -- this is just a translation by scrollDelta.
    //     Step 3. transform back to target surface space.
    //           -- this transform is the "partialLayerOriginTransform" = (parentMatrix * scale(layer->pageScaleDelta()));
    //
    // These steps create a matrix that both start and end in targetSurfaceSpace. So this matrix can
    // pre-multiply any fixed-position layer's drawTransform to undo the scrollDeltas -- as long as
    // that fixed position layer is fixed onto the same targetRenderSurface as this scrollingLayer.
    //

    WebTransformationMatrix partialLayerOriginTransform = parentMatrix;
    partialLayerOriginTransform.scale(scrollingLayer->pageScaleDelta());

    WebTransformationMatrix scrollCompensationForThisLayer = partialLayerOriginTransform; // Step 3
    scrollCompensationForThisLayer.translate(scrollingLayer->scrollDelta().width(), scrollingLayer->scrollDelta().height()); // Step 2
    scrollCompensationForThisLayer.multiply(partialLayerOriginTransform.inverse()); // Step 1
    return scrollCompensationForThisLayer;
}

WebTransformationMatrix computeScrollCompensationMatrixForChildren(LayerChromium* currentLayer, const WebTransformationMatrix& currentParentMatrix, const WebTransformationMatrix& currentScrollCompensation)
{
    // The main thread (i.e. LayerChromium) does not need to worry about scroll compensation.
    // So we can just return an identity matrix here.
    return WebTransformationMatrix();
}

WebTransformationMatrix computeScrollCompensationMatrixForChildren(CCLayerImpl* layer, const WebTransformationMatrix& parentMatrix, const WebTransformationMatrix& currentScrollCompensationMatrix)
{
    // "Total scroll compensation" is the transform needed to cancel out all scrollDelta translations that
    // occurred since the nearest container layer, even if there are renderSurfaces in-between.
    //
    // There are some edge cases to be aware of, that are not explicit in the code:
    //  - A layer that is both a fixed-position and container should not be its own container, instead, that means
    //    it is fixed to an ancestor, and is a container for any fixed-position descendants.
    //  - A layer that is a fixed-position container and has a renderSurface should behave the same as a container
    //    without a renderSurface, the renderSurface is irrelevant in that case.
    //  - A layer that does not have an explicit container is simply fixed to the viewport
    //    (i.e. the root renderSurface, and it would still compensate for root layer's scrollDelta).
    //  - If the fixed-position layer has its own renderSurface, then the renderSurface is
    //    the one who gets fixed.
    //
    // This function needs to be called AFTER layers create their own renderSurfaces.
    //

    // Avoid the overheads (including stack allocation and matrix initialization/copy) if we know that the scroll compensation doesn't need to be reset or adjusted.
    if (!layer->isContainerForFixedPositionLayers() && layer->scrollDelta().isZero() && !layer->renderSurface())
        return currentScrollCompensationMatrix;

    // Start as identity matrix.
    WebTransformationMatrix nextScrollCompensationMatrix;

    // If this layer is not a container, then it inherits the existing scroll compensations.
    if (!layer->isContainerForFixedPositionLayers())
        nextScrollCompensationMatrix = currentScrollCompensationMatrix;

    // If the current layer has a non-zero scrollDelta, then we should compute its local scrollCompensation
    // and accumulate it to the nextScrollCompensationMatrix.
    if (!layer->scrollDelta().isZero()) {
        WebTransformationMatrix scrollCompensationForThisLayer = computeScrollCompensationForThisLayer(layer, parentMatrix);
        nextScrollCompensationMatrix.multiply(scrollCompensationForThisLayer);
    }

    // If the layer created its own renderSurface, we have to adjust nextScrollCompensationMatrix.
    // The adjustment allows us to continue using the scrollCompensation on the next surface.
    //  Step 1 (right-most in the math): transform from the new surface to the original ancestor surface
    //  Step 2: apply the scroll compensation
    //  Step 3: transform back to the new surface.
    if (layer->renderSurface() && !nextScrollCompensationMatrix.isIdentity())
        nextScrollCompensationMatrix = layer->renderSurface()->originTransform().inverse() * nextScrollCompensationMatrix * layer->renderSurface()->originTransform();

    return nextScrollCompensationMatrix;
}

// Recursively walks the layer tree starting at the given node and computes all the
// necessary transformations, clipRects, render surfaces, etc.
template<typename LayerType, typename LayerList, typename RenderSurfaceType, typename LayerSorter>
static bool calculateDrawTransformsInternal(LayerType* layer, LayerType* rootLayer, const WebTransformationMatrix& parentMatrix,
                                            const WebTransformationMatrix& fullHierarchyMatrix, const WebTransformationMatrix& currentScrollCompensationMatrix,
                                            RenderSurfaceType* nearestAncestorThatMovesPixels, LayerList& renderSurfaceLayerList, LayerList& layerList,
                                            LayerSorter* layerSorter, int maxTextureSize)
{
    // This function computes the new matrix transformations recursively for this
    // layer and all its descendants. It also computes the appropriate render surfaces.
    // Some important points to remember:
    //
    // 0. Here, transforms are notated in Matrix x Vector order, and in words we describe what
    //    the transform does from left to right.
    //
    // 1. In our terminology, the "layer origin" refers to the top-left corner of a layer, and the
    //    positive Y-axis points downwards. This interpretation is valid because the orthographic
    //    projection applied at draw time flips the Y axis appropriately.
    //
    // 2. The anchor point, when given as a FloatPoint object, is specified in "unit layer space",
    //    where the bounds of the layer map to [0, 1]. However, as a WebTransformationMatrix object,
    //    the transform to the anchor point is specified in "pixel layer space", where the bounds
    //    of the layer map to [bounds.width(), bounds.height()].
    //
    // 3. The value of layer->position() is actually the position of the anchor point with respect to the position
    //    of the layer's origin. That is:
    //        layer->position() = positionOfLayerOrigin + anchorPoint (in pixel units)
    //
    //    Or, equivalently,
    //        positionOfLayerOrigin.x =  layer->position.x - (layer->anchorPoint.x * bounds.width)
    //        positionOfLayerOrigin.y =  layer->position.y - (layer->anchorPoint.y * bounds.height)
    //
    // 4. Definition of various transforms used:
    //        M[parent] is the parent matrix, with respect to the nearest render surface, passed down recursively.
    //        M[root] is the full hierarchy, with respect to the root, passed down recursively.
    //        Tr[origin] is the translation matrix from the parent's origin to this layer's origin.
    //        Tr[origin2anchor] is the translation from the layer's origin to its anchor point
    //        Tr[origin2center] is the translation from the layer's origin to its center
    //        M[layer] is the layer's matrix (applied at the anchor point)
    //        M[sublayer] is the layer's sublayer transform (applied at the layer's center)
    //        Tr[anchor2center] is the translation offset from the anchor point and the center of the layer
    //
    //    Some shortcuts and substitutions are used in the code to reduce matrix multiplications:
    //        Translating by the value of layer->position(), Tr[layer->position()] = Tr[origin] * Tr[origin2anchor]
    //        Tr[anchor2center] = Tr[origin2anchor].inverse() * Tr[origin2center]
    //
    //    Some composite transforms can help in understanding the sequence of transforms:
    //        compositeLayerTransform = Tr[origin2anchor] * M[layer] * Tr[origin2anchor].inverse()
    //        compositeSublayerTransform = Tr[origin2center] * M[sublayer] * Tr[origin2center].inverse()
    //
    //    In words, the layer transform is applied about the anchor point, and the sublayer transform is
    //    applied about the center of the layer.
    //
    // 5. When a layer (or render surface) is drawn, it is drawn into a "target render surface". Therefore the draw
    //    transform does not necessarily transform from screen space to local layer space. Instead, the draw transform
    //    is the transform between the "target render surface space" and local layer space. Note that render surfaces,
    //    except for the root, also draw themselves into a different target render surface, and so their draw
    //    transform and origin transforms are also described with respect to the target.
    //
    // Using these definitions, then:
    //
    // The draw transform for the layer is:
    //        M[draw] = M[parent] * Tr[origin] * compositeLayerTransform * Tr[origin2center]
    //                = M[parent] * Tr[layer->position()] * M[layer] * Tr[anchor2center]
    //
    //        Interpreting the math left-to-right, this transforms from the layer's render surface to the center of the layer.
    //
    // The screen space transform is:
    //        M[screenspace] = M[root] * Tr[origin] * compositeLayerTransform
    //                       = M[root] * Tr[layer->position()] * M[layer] * Tr[origin2anchor].inverse()
    //
    //        Interpreting the math left-to-right, this transforms from the root layer space to the local layer's origin.
    //
    // The transform hierarchy that is passed on to children (i.e. the child's parentMatrix) is:
    //        M[parent]_for_child = M[parent] * Tr[origin] * compositeLayerTransform * compositeSublayerTransform
    //                            = M[parent] * Tr[layer->position()] * M[layer] * Tr[anchor2center] * M[sublayer] * Tr[origin2center].inverse()
    //                            = M[draw] * M[sublayer] * Tr[origin2center].inverse()
    //
    //        and a similar matrix for the full hierarchy with respect to the root.
    //
    // Finally, note that the final matrix used by the shader for the layer is P * M[draw] * S . This final product
    // is computed in drawTexturedQuad(), where:
    //        P is the projection matrix
    //        S is the scale adjustment (to scale up to the layer size)
    //
    // When a render surface has a replica layer, that layer's transform is used to draw a second copy of the surface.
    // Transforms named here are relative to the surface, unless they specify they are relative to the replica layer.
    //
    // We will denote a scale by contents scale S[contentsScale]
    //
    // The render surface origin transform to its target surface origin is:
    //        M[surfaceOrigin] = M[owningLayer->Draw] * S[contentsScale].inverse() * Tr[origin2centerInScreenSpace].inverse()
    //
    // The render surface origin transform to its the root (screen space) origin is:
    //        M[surface2root] =  M[owningLayer->screenspace] * S[contentsScale].inverse()
    //
    // The replica draw transform is:
    //        M[replicaDraw] = M[surfaceOrigin] * S[contentsScale] * Tr[replica->position()] * Tr[replica] * Tr[anchor2center] * S[contentsScale].inverse()
    //                       = M[owningLayer->draw] * Tr[origin2center].inverse() * S[contentsScale] * Tr[replica->position()] * Tr[replica] * Tr[anchor2clippedCenter] * S[contentsScale].inverse()
    //
    // The replica origin transform to its target surface origin is:
    //        M[replicaOrigin] = S[contentsScale] * M[surfaceOrigin] * Tr[replica->position()] * Tr[replica] * Tr[origin2anchor].inverse() * S[contentsScale].invers()
    //
    // The replica origin transform to the root (screen space) origin is:
    //        M[replica2root] = M[surface2root] * Tr[replica->position()] * Tr[replica] * Tr[origin2anchor].inverse()
    //

    if (subtreeShouldBeSkipped(layer))
        return false;

    float drawOpacity = layer->opacity();
    bool drawOpacityIsAnimating = layer->opacityIsAnimating();
    if (layer->parent() && layer->parent()->preserves3D()) {
        drawOpacity *= layer->parent()->drawOpacity();
        drawOpacityIsAnimating |= layer->parent()->drawOpacityIsAnimating();
    }

    IntSize bounds = layer->bounds();
    FloatPoint anchorPoint = layer->anchorPoint();
    FloatPoint position = layer->position() - layer->scrollDelta();

    // Offset between anchor point and the center of the quad.
    float centerOffsetX = (0.5 - anchorPoint.x()) * bounds.width();
    float centerOffsetY = (0.5 - anchorPoint.y()) * bounds.height();

    WebTransformationMatrix layerLocalTransform;
    // LT = Tr[origin] * S[pageScaleDelta]
    layerLocalTransform.scale(layer->pageScaleDelta());
    // LT = Tr[origin] * S[pageScaleDelta] * Tr[origin2anchor]
    layerLocalTransform.translate3d(position.x(), position.y(), layer->anchorPointZ());
    // LT = Tr[origin] * S[pageScaleDelta] * Tr[origin2anchor] * M[layer]
    layerLocalTransform.multiply(layer->transform());
    // LT = Tr[origin] * S[pageScaleDelta] * Tr[origin2anchor] * M[layer] * Tr[anchor2center]
    layerLocalTransform.translate3d(centerOffsetX, centerOffsetY, -layer->anchorPointZ());

    // The combinedTransform that gets computed below is effectively the layer's drawTransform, unless
    // the layer itself creates a renderSurface. In that case, the renderSurface re-parents the transforms.
    WebTransformationMatrix combinedTransform = parentMatrix;
    combinedTransform.multiply(layerLocalTransform);

    if (layer->fixedToContainerLayer()) {
        // Special case: this layer is a composited fixed-position layer; we need to
        // explicitly compensate for all ancestors' nonzero scrollDeltas to keep this layer
        // fixed correctly.
        combinedTransform = currentScrollCompensationMatrix * combinedTransform;
    }

    bool animatingTransformToTarget = layer->transformIsAnimating();
    bool animatingTransformToScreen = animatingTransformToTarget;
    if (layer->parent()) {
        animatingTransformToTarget |= layer->parent()->drawTransformIsAnimating();
        animatingTransformToScreen |= layer->parent()->screenSpaceTransformIsAnimating();
    }

    float contentsScale = layer->contentsScale();

    FloatRect layerRect(-0.5 * layer->bounds().width(), -0.5 * layer->bounds().height(), layer->bounds().width(), layer->bounds().height());
    IntRect transformedLayerRect;

    // fullHierarchyMatrix is the matrix that transforms objects between screen space (except projection matrix) and the most recent RenderSurface's space.
    // nextHierarchyMatrix will only change if this layer uses a new RenderSurface, otherwise remains the same.
    WebTransformationMatrix nextHierarchyMatrix = fullHierarchyMatrix;

    // FIXME: This seems like the wrong place to set this
    layer->setUsesLayerClipping(false);

    if (subtreeShouldRenderToSeparateSurface(layer, isScaleOrTranslation(combinedTransform))) {
        // Check back-face visibility before continuing with this surface and its subtree
        if (!layer->doubleSided() && transformToParentIsKnown(layer) && isSurfaceBackFaceVisible(layer, combinedTransform))
            return false;

        if (!layer->renderSurface())
            layer->createRenderSurface();

        RenderSurfaceType* renderSurface = layer->renderSurface();
        renderSurface->clearLayerList();

        // The origin of the new surface is the upper left corner of the layer.
        WebTransformationMatrix drawTransform;
        drawTransform.scale(contentsScale);
        drawTransform.translate3d(0.5 * bounds.width(), 0.5 * bounds.height(), 0);
        layer->setDrawTransform(drawTransform);

        transformedLayerRect = IntRect(0, 0, contentsScale * bounds.width(), contentsScale * bounds.height());

        // The opacity value is moved from the layer to its surface, so that the entire subtree properly inherits opacity.
        renderSurface->setDrawOpacity(drawOpacity);
        renderSurface->setDrawOpacityIsAnimating(drawOpacityIsAnimating);
        layer->setDrawOpacity(1);
        layer->setDrawOpacityIsAnimating(false);

        WebTransformationMatrix surfaceOriginTransform = combinedTransform;
        // The surfaceOriginTransform transforms points in the surface's content space
        // to its parent's content space. Distances in these spaces are both in physical
        // pixels, so we need to 'undo' the scale by contentsScale. Ultimately, the
        // transform should map (0, 0) to contentsScale * position, and preserve distances.
        // Note, the following two lines are not equivalent to translating by (bounds.width(),
        // bounds.height). The effect on m41 and m42 would be identical, but the scale
        // affects the entire matrix. We need to scale these other entries to avoid
        // double scaling; we must remain in physical pixels.
        surfaceOriginTransform.scale(1 / contentsScale);
        surfaceOriginTransform.translate3d(-0.5 * transformedLayerRect.width(), -0.5 * transformedLayerRect.height(), 0);
        renderSurface->setOriginTransform(surfaceOriginTransform);

        renderSurface->setTargetSurfaceTransformsAreAnimating(animatingTransformToTarget);
        renderSurface->setScreenSpaceTransformsAreAnimating(animatingTransformToScreen);
        animatingTransformToTarget = false;
        layer->setDrawTransformIsAnimating(animatingTransformToTarget);
        layer->setScreenSpaceTransformIsAnimating(animatingTransformToScreen);

        // Update the aggregate hierarchy matrix to include the transform of the newly created RenderSurface.
        nextHierarchyMatrix.multiply(surfaceOriginTransform);

        // The render surface clipRect contributes to the scissor rect that needs to
        // be applied before drawing the render surface onto its containing
        // surface and is therefore expressed in the parent's coordinate system.
        renderSurface->setClipRect(layer->parent() ? layer->parent()->clipRect() : layer->clipRect());

        // The layer's clipRect can be reset here. The renderSurface will correctly clip the subtree.
        layer->setUsesLayerClipping(false);
        layer->setClipRect(IntRect());

        if (layer->maskLayer())
            layer->maskLayer()->setTargetRenderSurface(renderSurface);

        if (layer->replicaLayer() && layer->replicaLayer()->maskLayer())
            layer->replicaLayer()->maskLayer()->setTargetRenderSurface(renderSurface);

        renderSurface->setFilters(layer->filters());
        if (renderSurface->filters().hasFilterThatMovesPixels())
            nearestAncestorThatMovesPixels = renderSurface;
        renderSurface->setNearestAncestorThatMovesPixels(nearestAncestorThatMovesPixels);

        renderSurface->setBackgroundFilters(layer->backgroundFilters());

        renderSurfaceLayerList.append(layer);
    } else {
        layer->setDrawTransform(combinedTransform);
        layer->setDrawTransformIsAnimating(animatingTransformToTarget);
        layer->setScreenSpaceTransformIsAnimating(animatingTransformToScreen);
        transformedLayerRect = enclosingIntRect(CCMathUtil::mapClippedRect(layer->drawTransform(), layerRect));

        layer->setDrawOpacity(drawOpacity);
        layer->setDrawOpacityIsAnimating(drawOpacityIsAnimating);

        if (layer != rootLayer) {
            ASSERT(layer->parent());
            layer->clearRenderSurface();

            // Layers inherit the clip rect from their parent.
            layer->setClipRect(layer->parent()->clipRect());
            if (layer->parent()->usesLayerClipping())
                layer->setUsesLayerClipping(true);

            // Layers without their own renderSurface will render into the nearest ancestor surface.
            layer->setTargetRenderSurface(layer->parent()->targetRenderSurface());
        }
    }

    if (layer->masksToBounds()) {
        IntRect clipRect = transformedLayerRect;

        // If the layer already inherited a clipRect, we need to intersect with it before
        // overriding the layer's clipRect and usesLayerClipping.
        if (layer->usesLayerClipping())
            clipRect.intersect(layer->clipRect());

        layer->setClipRect(clipRect);
        layer->setUsesLayerClipping(true);
    }

    // Note that at this point, layer->drawTransform() is not necessarily the same as local variable drawTransform.
    // layerScreenSpaceTransform represents the transform between root layer's "screen space" and local layer space.
    WebTransformationMatrix layerScreenSpaceTransform = nextHierarchyMatrix;
    layerScreenSpaceTransform.multiply(layer->drawTransform());
    layerScreenSpaceTransform.translate3d(-0.5 * bounds.width(), -0.5 * bounds.height(), 0);
    layer->setScreenSpaceTransform(layerScreenSpaceTransform);

    // drawableContentRect() is always stored in the coordinate system of the
    // RenderSurface the layer draws into.
    if (layer->drawsContent()) {
        IntRect drawableContentRect = transformedLayerRect;
        if (layer->usesLayerClipping())
            drawableContentRect.intersect(layer->clipRect());
        layer->setDrawableContentRect(drawableContentRect);
    } else
        layer->setDrawableContentRect(IntRect());

    WebTransformationMatrix sublayerMatrix = layer->drawTransform();

    // Flatten to 2D if the layer doesn't preserve 3D.
    if (!layer->preserves3D()) {
        sublayerMatrix.setM13(0);
        sublayerMatrix.setM23(0);
        sublayerMatrix.setM31(0);
        sublayerMatrix.setM32(0);
        sublayerMatrix.setM33(1);
        sublayerMatrix.setM34(0);
        sublayerMatrix.setM43(0);
    }

    // Apply the sublayer transform at the center of the layer.
    sublayerMatrix.multiply(layer->sublayerTransform());

    // The coordinate system given to children is located at the layer's origin, not the center.
    sublayerMatrix.translate3d(-bounds.width() * 0.5, -bounds.height() * 0.5, 0);

    LayerList& descendants = (layer->renderSurface() ? layer->renderSurface()->layerList() : layerList);

    // Any layers that are appended after this point are in the layer's subtree and should be included in the sorting process.
    unsigned sortingStartIndex = descendants.size();

    if (!layerShouldBeSkipped(layer))
        descendants.append(layer);

    WebTransformationMatrix nextScrollCompensationMatrix = computeScrollCompensationMatrixForChildren(layer, parentMatrix, currentScrollCompensationMatrix);;

    for (size_t i = 0; i < layer->children().size(); ++i) {
        LayerType* child = layer->children()[i].get();
        bool drawsContent = calculateDrawTransformsInternal<LayerType, LayerList, RenderSurfaceType, LayerSorter>(child, rootLayer, sublayerMatrix, nextHierarchyMatrix, nextScrollCompensationMatrix, nearestAncestorThatMovesPixels, renderSurfaceLayerList, descendants, layerSorter, maxTextureSize);

        if (drawsContent) {
            if (child->renderSurface()) {
                RenderSurfaceType* childRenderSurface = child->renderSurface();
                IntRect drawableContentRect = layer->drawableContentRect();
                drawableContentRect.unite(enclosingIntRect(childRenderSurface->drawableContentRect()));
                layer->setDrawableContentRect(drawableContentRect);
                descendants.append(child);
            } else {
                IntRect drawableContentRect = layer->drawableContentRect();
                drawableContentRect.unite(child->drawableContentRect());
                layer->setDrawableContentRect(drawableContentRect);
            }
        }
    }

    if (layer->masksToBounds() || layer->maskLayer()) {
        IntRect drawableContentRect = layer->drawableContentRect();
        drawableContentRect.intersect(transformedLayerRect);
        layer->setDrawableContentRect(drawableContentRect);
    }

    if (layer->renderSurface() && layer != rootLayer) {
        RenderSurfaceType* renderSurface = layer->renderSurface();
        IntRect clippedContentRect = layer->drawableContentRect();
        FloatPoint surfaceCenter = FloatRect(clippedContentRect).center();

        // Restrict the RenderSurface size to the portion that's visible.
        FloatSize centerOffsetDueToClipping;

        // Don't clip if the layer is reflected as the reflection shouldn't be
        // clipped. If the layer is animating, then the surface's transform to
        // its target is not known on the main thread, and we should not use it
        // to clip.
        if (!layer->replicaLayer() && transformToParentIsKnown(layer)) {
            if (!renderSurface->clipRect().isEmpty() && !clippedContentRect.isEmpty()) {
                IntRect surfaceClipRect = CCLayerTreeHostCommon::calculateVisibleRect(renderSurface->clipRect(), clippedContentRect, renderSurface->originTransform());
                clippedContentRect.intersect(surfaceClipRect);
            }
            FloatPoint clippedSurfaceCenter = FloatRect(clippedContentRect).center();
            centerOffsetDueToClipping = clippedSurfaceCenter - surfaceCenter;
        }

        // The RenderSurface backing texture cannot exceed the maximum supported
        // texture size.
        clippedContentRect.setWidth(std::min(clippedContentRect.width(), maxTextureSize));
        clippedContentRect.setHeight(std::min(clippedContentRect.height(), maxTextureSize));

        if (clippedContentRect.isEmpty())
            renderSurface->clearLayerList();

        renderSurface->setContentRect(clippedContentRect);

        // Since the layer starts a new render surface we need to adjust its
        // clipRect to be expressed in the new surface's coordinate system.
        layer->setClipRect(layer->drawableContentRect());

        // Adjust the origin of the transform to be the center of the render surface.
        WebTransformationMatrix drawTransform = renderSurface->originTransform();
        drawTransform.translate3d(surfaceCenter.x() + centerOffsetDueToClipping.width(), surfaceCenter.y() + centerOffsetDueToClipping.height(), 0);
        renderSurface->setDrawTransform(drawTransform);

        WebTransformationMatrix screenSpaceTransform = layer->screenSpaceTransform();
        // The layer's screen space transform operates on layer rects, but the surfaces
        // screen space transform operates on surface rects, which are in physical pixels,
        // so we have to 'undo' the scale here.
        screenSpaceTransform.scale(1 / contentsScale);
        renderSurface->setScreenSpaceTransform(screenSpaceTransform);

        if (layer->replicaLayer()) {
            // Compute the transformation matrix used to draw the surface's replica to the target surface.
            WebTransformationMatrix replicaDrawTransform = renderSurface->originTransform();

            replicaDrawTransform.scale(contentsScale);
            replicaDrawTransform.translate(layer->replicaLayer()->position().x(), layer->replicaLayer()->position().y());
            replicaDrawTransform.multiply(layer->replicaLayer()->transform());
            FloatPoint layerSpaceSurfaceCenter = surfaceCenter;
            layerSpaceSurfaceCenter.scale(1 / contentsScale, 1 / contentsScale);
            replicaDrawTransform.translate(layerSpaceSurfaceCenter.x() - anchorPoint.x() * bounds.width(), layerSpaceSurfaceCenter.y() - anchorPoint.y() * bounds.height());
            replicaDrawTransform.scale(1 / contentsScale);

            renderSurface->setReplicaDrawTransform(replicaDrawTransform);

            WebTransformationMatrix surfaceOriginToReplicaOriginTransform;
            surfaceOriginToReplicaOriginTransform.scale(contentsScale);
            surfaceOriginToReplicaOriginTransform.translate(layer->replicaLayer()->position().x(), layer->replicaLayer()->position().y());
            surfaceOriginToReplicaOriginTransform.multiply(layer->replicaLayer()->transform());
            surfaceOriginToReplicaOriginTransform.translate(-anchorPoint.x() * bounds.width(), -anchorPoint.y() * bounds.height());
            surfaceOriginToReplicaOriginTransform.scale(1 / contentsScale);

            // Compute the replica's "originTransform" that maps from the replica's origin space to the target surface origin space.
            WebTransformationMatrix replicaOriginTransform = layer->renderSurface()->originTransform() * surfaceOriginToReplicaOriginTransform;
            renderSurface->setReplicaOriginTransform(replicaOriginTransform);

            // Compute the replica's "screenSpaceTransform" that maps from the replica's origin space to the screen's origin space.
            WebTransformationMatrix replicaScreenSpaceTransform = layer->renderSurface()->screenSpaceTransform() * surfaceOriginToReplicaOriginTransform;
            renderSurface->setReplicaScreenSpaceTransform(replicaScreenSpaceTransform);
        }

        // If a render surface has no layer list, then it and none of its children needed to get drawn.
        if (!layer->renderSurface()->layerList().size()) {
            // FIXME: Originally we asserted that this layer was already at the end of the
            //        list, and only needed to remove that layer. For now, we remove the
            //        entire subtree of surfaces to fix a crash bug. The root cause is
            //        https://bugs.webkit.org/show_bug.cgi?id=74147 and we should be able
            //        to put the original assert after fixing that.
            while (renderSurfaceLayerList.last() != layer) {
                renderSurfaceLayerList.last()->clearRenderSurface();
                renderSurfaceLayerList.removeLast();
            }
            ASSERT(renderSurfaceLayerList.last() == layer);
            renderSurfaceLayerList.removeLast();
            layer->clearRenderSurface();
            return false;
        }
    }

    // If neither this layer nor any of its children were added, early out.
    if (sortingStartIndex == descendants.size())
        return false;

    // If preserves-3d then sort all the descendants in 3D so that they can be
    // drawn from back to front. If the preserves-3d property is also set on the parent then
    // skip the sorting as the parent will sort all the descendants anyway.
    if (descendants.size() && layer->preserves3D() && (!layer->parent() || !layer->parent()->preserves3D()))
        sortLayers(&descendants.at(sortingStartIndex), descendants.end(), layerSorter);

    return true;
}

// FIXME: Instead of using the following function to set visibility rects on a second
// tree pass, revise calculateVisibleLayerRect() so that this can be done in a single
// pass inside calculateDrawTransformsInternal<>().
template<typename LayerType, typename LayerList, typename RenderSurfaceType>
static void calculateVisibleAndScissorRectsInternal(const LayerList& renderSurfaceLayerList, const FloatRect& rootScissorRect)
{
    // Use BackToFront since it's cheap and this isn't order-dependent.
    typedef CCLayerIterator<LayerType, LayerList, RenderSurfaceType, CCLayerIteratorActions::BackToFront> CCLayerIteratorType;

    CCLayerIteratorType end = CCLayerIteratorType::end(&renderSurfaceLayerList);
    for (CCLayerIteratorType it = CCLayerIteratorType::begin(&renderSurfaceLayerList); it != end; ++it) {
        if (it.representsTargetRenderSurface()) {
            LayerType* maskLayer = it->maskLayer();
            if (maskLayer)
                maskLayer->setVisibleLayerRect(IntRect(IntPoint(), it->contentBounds()));
            LayerType* replicaMaskLayer = it->replicaLayer() ? it->replicaLayer()->maskLayer() : 0;
            if (replicaMaskLayer)
                replicaMaskLayer->setVisibleLayerRect(IntRect(IntPoint(), it->contentBounds()));
        } else if (it.representsItself()) {
            IntRect visibleLayerRect = calculateVisibleLayerRect(*it);
            it->setVisibleLayerRect(visibleLayerRect);

            IntRect scissorRect = calculateLayerScissorRect<LayerType, RenderSurfaceType>(*it, rootScissorRect);
            it->setScissorRect(scissorRect);
        } else if (it.representsContributingRenderSurface()) {
            IntRect scissorRect = calculateSurfaceScissorRect<LayerType, RenderSurfaceType>(*it, rootScissorRect);
            it->renderSurface()->setScissorRect(scissorRect);
        }
    }
}

void CCLayerTreeHostCommon::calculateDrawTransforms(LayerChromium* layer, LayerChromium* rootLayer, const WebTransformationMatrix& parentMatrix, const WebTransformationMatrix& fullHierarchyMatrix, Vector<RefPtr<LayerChromium> >& renderSurfaceLayerList, Vector<RefPtr<LayerChromium> >& layerList, int maxTextureSize)
{
    WebTransformationMatrix scrollCompensationMatrix;
    WebCore::calculateDrawTransformsInternal<LayerChromium, Vector<RefPtr<LayerChromium> >, RenderSurfaceChromium, void>(layer, rootLayer, parentMatrix, fullHierarchyMatrix, scrollCompensationMatrix, 0, renderSurfaceLayerList, layerList, 0, maxTextureSize);
}

void CCLayerTreeHostCommon::calculateDrawTransforms(CCLayerImpl* layer, CCLayerImpl* rootLayer, const WebTransformationMatrix& parentMatrix, const WebTransformationMatrix& fullHierarchyMatrix, Vector<CCLayerImpl*>& renderSurfaceLayerList, Vector<CCLayerImpl*>& layerList, CCLayerSorter* layerSorter, int maxTextureSize)
{
    WebTransformationMatrix scrollCompensationMatrix;
    WebCore::calculateDrawTransformsInternal<CCLayerImpl, Vector<CCLayerImpl*>, CCRenderSurface, CCLayerSorter>(layer, rootLayer, parentMatrix, fullHierarchyMatrix, scrollCompensationMatrix, 0, renderSurfaceLayerList, layerList, layerSorter, maxTextureSize);
}

void CCLayerTreeHostCommon::calculateVisibleAndScissorRects(Vector<RefPtr<LayerChromium> >& renderSurfaceLayerList, const FloatRect& rootScissorRect)
{
    calculateVisibleAndScissorRectsInternal<LayerChromium, Vector<RefPtr<LayerChromium> >, RenderSurfaceChromium>(renderSurfaceLayerList, rootScissorRect);
}

void CCLayerTreeHostCommon::calculateVisibleAndScissorRects(Vector<CCLayerImpl*>& renderSurfaceLayerList, const FloatRect& rootScissorRect)
{
    calculateVisibleAndScissorRectsInternal<CCLayerImpl, Vector<CCLayerImpl*>, CCRenderSurface>(renderSurfaceLayerList, rootScissorRect);
}

static bool pointHitsRect(const IntPoint& viewportPoint, const WebTransformationMatrix& localSpaceToScreenSpaceTransform, FloatRect localSpaceRect)
{
    // If the transform is not invertible, then assume that this point doesn't hit this rect.
    if (!localSpaceToScreenSpaceTransform.isInvertible())
        return false;

    // Transform the hit test point from screen space to the local space of the given rect.
    bool clipped = false;
    FloatPoint hitTestPointInLocalSpace = CCMathUtil::projectPoint(localSpaceToScreenSpaceTransform.inverse(), FloatPoint(viewportPoint), clipped);

    // If projectPoint could not project to a valid value, then we assume that this point doesn't hit this rect.
    if (clipped)
        return false;

    return localSpaceRect.contains(hitTestPointInLocalSpace);
}

static bool pointIsClippedBySurfaceOrClipRect(const IntPoint& viewportPoint, CCLayerImpl* layer)
{
    CCLayerImpl* currentLayer = layer;

    // Walk up the layer tree and hit-test any renderSurfaces and any layer clipRects that are active.
    while (currentLayer) {
        if (currentLayer->renderSurface() && !pointHitsRect(viewportPoint, currentLayer->renderSurface()->screenSpaceTransform(), currentLayer->renderSurface()->contentRect()))
            return true;

        // Note that clipRects are actually in targetSurface space, so the transform we
        // have to provide is the target surface's screenSpaceTransform.
        if (currentLayer->usesLayerClipping() && !pointHitsRect(viewportPoint, currentLayer->targetRenderSurface()->screenSpaceTransform(), currentLayer->clipRect()))
            return true;

        currentLayer = currentLayer->parent();
    }

    // If we have finished walking all ancestors without having already exited, then the point is not clipped by any ancestors.
    return false;
}

CCLayerImpl* CCLayerTreeHostCommon::findLayerThatIsHitByPoint(const IntPoint& viewportPoint, Vector<CCLayerImpl*>& renderSurfaceLayerList)
{
    CCLayerImpl* foundLayer = 0;

    typedef CCLayerIterator<CCLayerImpl, Vector<CCLayerImpl*>, CCRenderSurface, CCLayerIteratorActions::FrontToBack> CCLayerIteratorType;
    CCLayerIteratorType end = CCLayerIteratorType::end(&renderSurfaceLayerList);

    for (CCLayerIteratorType it = CCLayerIteratorType::begin(&renderSurfaceLayerList); it != end; ++it) {
        // We don't want to consider renderSurfaces for hit testing.
        if (!it.representsItself())
            continue;

        CCLayerImpl* currentLayer = (*it);

        FloatRect layerRect(FloatPoint::zero(), currentLayer->bounds());
        if (!pointHitsRect(viewportPoint, currentLayer->screenSpaceTransform(), layerRect))
            continue;

        // At this point, we think the point does hit the layer, but we need to walk up
        // the parents to ensure that the layer was not clipped in such a way that the
        // hit point actually should not hit the layer.
        if (pointIsClippedBySurfaceOrClipRect(viewportPoint, currentLayer))
            continue;

        foundLayer = currentLayer;
        break;
    }

    // This can potentially return 0, which means the viewportPoint did not successfully hit test any layers, not even the root layer.
    return foundLayer;
}

} // namespace WebCore
