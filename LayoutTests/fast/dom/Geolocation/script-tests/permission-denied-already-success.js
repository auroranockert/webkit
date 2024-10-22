description("Tests that when Geolocation permission has been denied prior to a call to a Geolocation method, the error callback is invoked with code PERMISSION_DENIED, when the Geolocation service has a good position.");

// Prime the Geolocation instance by denying permission.
if (window.testRunner) {
    testRunner.setGeolocationPermission(false);
    testRunner.setMockGeolocationPosition(51.478, -0.166, 100);
} else
    debug('This test can not be run without the LayoutTestController');

var error;
navigator.geolocation.getCurrentPosition(function(p) {
    testFailed('Success callback invoked unexpectedly');
    finishJSTest();
}, function(e) {
    error = e;
    shouldBe('error.code', 'error.PERMISSION_DENIED');
    shouldBe('error.message', '"User denied Geolocation"');
    debug('');
    continueTest();
});

function continueTest()
{
    // Make another request, with permission already denied.
    navigator.geolocation.getCurrentPosition(function(p) {
        testFailed('Success callback invoked unexpectedly');
        finishJSTest();
    }, function(e) {
        error = e;
        shouldBe('error.code', 'error.PERMISSION_DENIED');
        shouldBe('error.message', '"User denied Geolocation"');
        finishJSTest();
    });
}

window.jsTestIsAsync = true;
