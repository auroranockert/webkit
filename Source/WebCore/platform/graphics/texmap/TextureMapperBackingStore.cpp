/*
 Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies)

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Library General Public
 License as published by the Free Software Foundation; either
 version 2 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Library General Public License for more details.

 You should have received a copy of the GNU Library General Public License
 along with this library; see the file COPYING.LIB.  If not, write to
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "TextureMapperBackingStore.h"

#include "GraphicsLayer.h"
#include "ImageBuffer.h"
#include "TextureMapper.h"

#if USE(GRAPHICS_SURFACE)
#include "GraphicsSurface.h"
#include "TextureMapperGL.h"
#endif

namespace WebCore {

#if USE(GRAPHICS_SURFACE)
void TextureMapperSurfaceBackingStore::setGraphicsSurface(uint32_t graphicsSurfaceToken, const IntSize& surfaceSize)
{
    if (graphicsSurfaceToken != m_backBufferGraphicsSurfaceData.m_graphicsSurfaceToken) {
        GraphicsSurface::Flags surfaceFlags = GraphicsSurface::SupportsTextureTarget
                                            | GraphicsSurface::SupportsSharing;
        m_backBufferGraphicsSurfaceData.setSurface(GraphicsSurface::create(surfaceSize, surfaceFlags, graphicsSurfaceToken));
        m_graphicsSurfaceSize = surfaceSize;
    }

    std::swap(m_backBufferGraphicsSurfaceData, m_frontBufferGraphicsSurfaceData);
}

PassRefPtr<BitmapTexture> TextureMapperSurfaceBackingStore::texture() const
{
    // FIXME: Instead of just returning an empty texture, we should wrap the texture contents into a BitmapTexture.
    RefPtr<BitmapTexture> emptyTexture;
    return emptyTexture;
}

void TextureMapperSurfaceBackingStore::paintToTextureMapper(TextureMapper* textureMapper, const FloatRect& targetRect, const TransformationMatrix& transform, float opacity, BitmapTexture* mask)
{
    TransformationMatrix adjustedTransform = transform;
    adjustedTransform.multiply(TransformationMatrix::rectToRect(FloatRect(FloatPoint::zero(), m_graphicsSurfaceSize), targetRect));
#if OS(DARWIN)
    // This is specific to the Mac implementation of GraphicsSurface. IOSurface requires GL_TEXTURE_RECTANGLE_ARB to be used.
    static_cast<TextureMapperGL*>(textureMapper)->drawTextureRectangleARB(m_frontBufferGraphicsSurfaceData.m_textureID, 0, m_graphicsSurfaceSize, targetRect, adjustedTransform, opacity, mask);
#else
    UNUSED_PARAM(textureMapper);
    UNUSED_PARAM(opacity);
    UNUSED_PARAM(mask);
#endif
}
#endif

void TextureMapperTile::updateContents(TextureMapper* textureMapper, Image* image, const IntRect& dirtyRect)
{
    IntRect targetRect = enclosingIntRect(m_rect);
    targetRect.intersect(dirtyRect);
    if (targetRect.isEmpty())
        return;
    IntPoint sourceOffset = targetRect.location();

    // Normalize sourceRect to the buffer's coordinates.
    sourceOffset.move(-dirtyRect.x(), -dirtyRect.y());

    // Normalize targetRect to the texture's coordinates.
    targetRect.move(-m_rect.x(), -m_rect.y());
    if (!m_texture) {
        m_texture = textureMapper->createTexture();
        m_texture->reset(targetRect.size(), image->currentFrameHasAlpha() ? BitmapTexture::SupportsAlpha : 0);
    }

    m_texture->updateContents(image, targetRect, sourceOffset);
}

void TextureMapperTile::paint(TextureMapper* textureMapper, const TransformationMatrix& transform, float opacity, BitmapTexture* mask)
{
    if (texture().get())
        textureMapper->drawTexture(*texture().get(), rect(), transform, opacity, mask);
}

TextureMapperTiledBackingStore::TextureMapperTiledBackingStore()
    : m_drawsDebugBorders(false)
{
}

void TextureMapperTiledBackingStore::updateContentsFromImageIfNeeded(TextureMapper* textureMapper)
{
    if (!m_image)
        return;

    updateContents(textureMapper, m_image.get());
    m_image.clear();
}

void TextureMapperTiledBackingStore::paintToTextureMapper(TextureMapper* textureMapper, const FloatRect& targetRect, const TransformationMatrix& transform, float opacity, BitmapTexture* mask)
{
    updateContentsFromImageIfNeeded(textureMapper);
    TransformationMatrix adjustedTransform = transform;
    adjustedTransform.multiply(TransformationMatrix::rectToRect(rect(), targetRect));
    for (size_t i = 0; i < m_tiles.size(); ++i) {
        m_tiles[i].paint(textureMapper, adjustedTransform, opacity, mask);
        if (m_drawsDebugBorders)
            textureMapper->drawBorder(m_debugBorderColor, m_debugBorderWidth, m_tiles[i].rect(), adjustedTransform);
    }
}

void TextureMapperTiledBackingStore::createOrDestroyTilesIfNeeded(const FloatSize& size, const IntSize& tileSize, bool hasAlpha)
{
    if (size == m_size)
        return;

    m_size = size;

    Vector<FloatRect> tileRectsToAdd;
    Vector<int> tileIndicesToRemove;
    static const size_t TileEraseThreshold = 6;

    // This method recycles tiles. We check which tiles we need to add, which to remove, and use as many
    // removable tiles as replacement for new tiles when possible.
    for (float y = 0; y < m_size.height(); y += tileSize.height()) {
        for (float x = 0; x < m_size.width(); x += tileSize.width()) {
            FloatRect tileRect(x, y, tileSize.width(), tileSize.height());
            tileRect.intersect(rect());
            tileRectsToAdd.append(tileRect);
        }
    }

    // Check which tiles need to be removed, and which already exist.
    for (int i = m_tiles.size() - 1; i >= 0; --i) {
        FloatRect oldTile = m_tiles[i].rect();
        bool existsAlready = false;

        for (int j = tileRectsToAdd.size() - 1; j >= 0; --j) {
            FloatRect newTile = tileRectsToAdd[j];
            if (oldTile != newTile)
                continue;

            // A tile that we want to add already exists, no need to add or remove it.
            existsAlready = true;
            tileRectsToAdd.remove(j);
            break;
        }

        // This tile is not needed.
        if (!existsAlready)
            tileIndicesToRemove.append(i);
    }

    // Recycle removable tiles to be used for newly requested tiles.
    for (size_t i = 0; i < tileRectsToAdd.size(); ++i) {
        if (!tileIndicesToRemove.isEmpty()) {
            // We recycle an existing tile for usage with a new tile rect.
            TextureMapperTile& tile = m_tiles[tileIndicesToRemove.last()];
            tileIndicesToRemove.removeLast();
            tile.setRect(tileRectsToAdd[i]);

            if (tile.texture())
                tile.texture()->reset(enclosingIntRect(tile.rect()).size(), hasAlpha ? BitmapTexture::SupportsAlpha : 0);
            continue;
        }

        m_tiles.append(TextureMapperTile(tileRectsToAdd[i]));
    }

    // Remove unnecessary tiles, if they weren't recycled.
    // We use a threshold to make sure we don't create/destroy tiles too eagerly.
    for (size_t i = 0; i < tileIndicesToRemove.size() && m_tiles.size() > TileEraseThreshold; ++i)
        m_tiles.remove(tileIndicesToRemove[i]);
}

void TextureMapperTiledBackingStore::updateContents(TextureMapper* textureMapper, Image* image, const FloatSize& totalSize, const IntRect& dirtyRect)
{
    createOrDestroyTilesIfNeeded(totalSize, textureMapper->maxTextureSize(), image->currentFrameHasAlpha());
    for (size_t i = 0; i < m_tiles.size(); ++i)
        m_tiles[i].updateContents(textureMapper, image, dirtyRect);
}

PassRefPtr<BitmapTexture> TextureMapperTiledBackingStore::texture() const
{
    for (size_t i = 0; i < m_tiles.size(); ++i) {
        RefPtr<BitmapTexture> texture = m_tiles[i].texture();
        if (texture)
            return texture;
    }

    return PassRefPtr<BitmapTexture>();
}

void TextureMapperTiledBackingStore::setDebugBorder(const Color& color, float width)
{
    m_debugBorderColor = color;
    m_debugBorderWidth = width;
}

}
