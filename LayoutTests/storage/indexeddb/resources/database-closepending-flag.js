if (this.importScripts) {
    importScripts('../../../fast/js/resources/js-test-pre.js');
    importScripts('shared.js');
}

description("Test IndexedDB IDBDatabase internal closePending flag");

function test()
{
    removeVendorPrefixes();

    evalAndLog("dbname = self.location.pathname");
    request = evalAndLog("indexedDB.deleteDatabase(dbname)");
    request.onerror = unexpectedErrorCallback;
    request.onsuccess = openConnection;
}

function openConnection()
{
    debug("");
    debug("openConnection():");
    evalAndLog("request = indexedDB.open(dbname)");
    request.onerror = unexpectedErrorCallback;
    request.onsuccess = function() {
        evalAndLog("connection = request.result");
        evalAndLog("request = connection.setVersion('1')");
        request.onerror = unexpectedErrorCallback;
        request.onsuccess = function() {
            trans = request.result;
            evalAndLog("store = connection.createObjectStore('store')");
            trans.onabort = unexpectedAbortCallback;
            trans.oncomplete = testDatabaseClosingSteps;
        };
    };
}

function testDatabaseClosingSteps()
{
    debug("");
    debug("First, verify that the database connection is not closed:");
    shouldNotThrow("transaction = connection.transaction('store')");
    transaction.objectStore('store').get(0); // FIXME: Can remove this line when WK89379 lands

    debug("");
    debug("Database closing steps");
    debug("\"1. Set the internal closePending flag of connection to true.\"");
    evalAndLog("connection.close()");
    evalAndExpectException("connection.transaction('store')", "DOMException.INVALID_STATE_ERR", "'InvalidStateError'");
    debug("\"2. Wait for all transactions created using connection to complete. Once they are complete, connection is closed.\"");

    evalAndLog("transaction.oncomplete = testIDBDatabaseName");
}

function testIDBDatabaseName()
{
    debug("");
    debug("IDBDatabase.name:");
    debug("\"The function must return this name even if the closePending flag is set on the connection.\"");
    shouldBe("connection.name", "dbname");

    testIDBDatabaseObjectStoreNames();
}

function testIDBDatabaseObjectStoreNames()
{
    debug("");
    debug("IDBDatabase.objectStoreNames:");
    debug("\"Once the closePending flag is set on the connection, this function must return a snapshot of the list of names of the object stores taken at the time when the close method was called.\"");

    evalAndLog("request = indexedDB.open(dbname)");
    request.onerror = unexpectedErrorCallback;
    request.onsuccess = function() {
        evalAndLog("version_change_connection = request.result");
        request = evalAndLog("version_change_connection.setVersion('2')");
        // FIXME: Should verify 'blocked' not fired. http://webkit.org/b/71130
        request.onerror = unexpectedErrorCallback;
        request.onsuccess = function () {
            var transaction = request.result;
            evalAndLog("version_change_connection.createObjectStore('new_store')");
            transaction.oncomplete = function () {
                shouldBeTrue("version_change_connection.objectStoreNames.contains('new_store')");
                shouldBeFalse("connection.objectStoreNames.contains('new_store')");
                evalAndLog("version_change_connection.close()");

                testIDBDatabaseTransaction();
            };
        };
    };
}

function testIDBDatabaseTransaction()
{
    debug("");
    debug("IDBDatabase.transaction():");
    debug("\"...if this method is called on a IDBDatabase instance where the closePending flag is set, a InvalidStateError exception must be thrown.\"");
    evalAndExpectException("connection.transaction('store')", "DOMException.INVALID_STATE_ERR", "'InvalidStateError'");

    testVersionChangeTransactionSteps();
}

function testVersionChangeTransactionSteps()
{
    debug("");
    debug("\"versionchange\" transaction steps:");
    debug("\"Fire a versionchange event at each object in openDatabases that is open. The event must not be fired on objects which has the closePending flag set.\"");

    evalAndLog("request = indexedDB.open(dbname)");
    request.onerror = unexpectedErrorCallback;
    request.onsuccess = function() {
        evalAndLog("connection = request.result");
        evalAndLog("versionChangeWasFired = false");
        evalAndLog("connection.onversionchange = function () { versionChangeWasFired = true; }");
        evalAndLog("transaction = connection.transaction('store')");
        evalAndLog("connection.close()");
        debug("closePending is set, but active transaction will keep connection from closing");

        keepTransactionBusy = true;
        function busy() {
            if (keepTransactionBusy)
                transaction.objectStore('store').get(0).onsuccess = busy;
        }
        busy();

        evalAndLog("request = indexedDB.open(dbname)");
        request.onerror = unexpectedErrorCallback;
        request.onsuccess = function() {
            evalAndLog("version_change_connection = request.result");
            request = evalAndLog("version_change_connection.setVersion('3')");
            request.onblocked = function () {
                debug("'blocked' event fired, letting transaction complete and connection close");
                keepTransactionBusy = false;
            };
            request.onerror = unexpectedErrorCallback;
            request.onsuccess = function () {
                shouldBeFalse("versionChangeWasFired");
                var transaction = request.result;
                transaction.oncomplete = function () {
                    request = evalAndLog("version_change_connection.close()");

                    testDatabaseDeletion();
                };
            };
        };
    };
}

function testDatabaseDeletion()
{
    debug("");
    debug("Database deletion steps:");
    debug("\"Fire a versionchange event at each object in openDatabases that is open. The event must not be fired on objects which has the closePending flag set.\"");

    evalAndLog("request = indexedDB.open(dbname)");
    request.onerror = unexpectedErrorCallback;
    request.onsuccess = function() {
        evalAndLog("connection = request.result");
        evalAndLog("versionChangeWasFired = false");
        evalAndLog("connection.onversionchange = function () { versionChangeWasFired = true; }");
        evalAndLog("transaction = connection.transaction('store')");
        evalAndLog("connection.close()");
        debug("closePending is set, but active transaction will keep connection from closing");

        keepTransactionBusy = true;
        function busy() {
            if (keepTransactionBusy)
                transaction.objectStore('store').get(0).onsuccess = busy;
        }
        busy();

        evalAndLog("request = indexedDB.deleteDatabase(dbname)");
        request.onblocked = function () {
            debug("'blocked' event fired, letting transaction complete and connection close");
            keepTransactionBusy = false;
        };
        request.onerror = unexpectedErrorCallback;
        request.onsuccess = function () {
            shouldBeFalse("versionChangeWasFired");
            finishJSTest();
        };
    };
}

test();
