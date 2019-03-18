import QtQuick 2.5
import de.skycoder42.RestClient 2.1
import QtTest 1.1
import de.skycoder42.qtrestclient.test 2.1

Item {
	id: root

	//TODO make full testcase via http server
	TestCase {
		id: testCase
		name: "RestClient"

		RestClient {
			id: api
			baseUrl: "http://localhost:%1".arg(testPort)

			Component.onCompleted: QtRestClient.addGlobalApi("testapi", api)

			RestClass {
				id: postClass
				path: "posts"
			}

			RestClass {
				id: pagesClass
				path: "pages"
			}

			RestClass {
				id: userClass
				path: "users"

				RestClass {
					id: userNameClass
					path: "name"
				}
			}
		}

		QtObject {
			id: waiter
			property bool ok: false
			property bool done: false
			property int counter: 0

			function jcomp(actual, expected) {
				compare(actual, expected, JSON.stringify(actual) + " != " + JSON.stringify(expected));
			}

			function repConnect(reply) {
				reply.failed.connect(function(code, data) {
					done = true;
					testCase.fail(code + ": " + JSON.stringify(data));
				});
				reply.error.connect(function(error, code, type) {
					done = true;
					testCase.fail(error + " (" + code + ", " + type + ")");
				});
			}

			function ewait(reply) {
				repConnect(reply);
				var i = 0;
				while(!done && i < 50){
					i++;
					testCase.wait(100);
				}
				done = false;
			}

			function cwait(reply, counter) {
				repConnect(reply);
				var i = 0;
				while(!done && waiter.counter < counter && i < 50){
					i++;
					testCase.wait(100);
				}
				done = false;
				ok = waiter.counter === counter;
				waiter.counter = 0;
			}
		}

		function initTestCase() {
			// check if all restclasses have been initialized
			verify(postClass.restClass);
			verify(userClass.restClass);
			verify(userNameClass.restClass);

			verify(QtRestClient.apiClient("testapi"));
			verify(QtRestClient.createApiClass("testapi", "some/sub/path", root));

			verify(postClass.get("1"));
			verify(userClass.put(["hello", "world"]));
			verify(userNameClass.post({
										  id: 42,
										  name: "user"
									  }));
		}

		function test_post() {
			waiter.ok = false;
			var params = {
				id: "1",
				userId: "42",
				title: "baum",
				body: "baum"
			};
			var reply = postClass.post(undefined, params);
			reply.succeeded.connect(function(code, data){
				waiter.done = true;
				compare(code, 200);
				compare(data, params, JSON.stringify(data) + " != " + JSON.stringify(params));
				waiter.ok = true;
			});
			waiter.ewait(reply);
			verify(waiter.ok);
		}

		function test_put() {
			waiter.ok = false;
			var obj = {
				id: 1,
				userId: 42,
				title: "baum",
				body: "baum"
			};
			var reply = postClass.put(obj, "1");
			reply.succeeded.connect(function(code, data){
				waiter.done = true;
				compare(code, 200);
				compare(data, obj, JSON.stringify(data) + " != " + JSON.stringify(obj));
				waiter.ok = true;
			});
			waiter.ewait(reply);
			verify(waiter.ok);
		}

		function test_list() {
			waiter.ok = false;
			var reply = postClass.get();
			reply.succeeded.connect(function(code, data){
				waiter.done = true;
				compare(code, 200);
				compare(data.length, 100);
				waiter.ok = true;
			});
			waiter.ewait(reply);
			verify(waiter.ok);
		}

		function test_paging() {
			waiter.ok = false;
			var reply = pagesClass.get("0");
			reply.completed.connect(function(code, data) {
				compare(code, 200);
				verify(data);
				var paging = QtRestClient.createPaging(api, data);
				verify(paging);
				compare(paging.total, 100);
				compare(paging.offset, waiter.counter);
				compare(paging.items.length, 10);
				paging.iterate(function(item, index){
					compare(item.id, waiter.counter);
					compare(index, waiter.counter++);
					return true;
				});
			});
			waiter.cwait(reply, 100);
			verify(waiter.ok);
		}
	}

	TestCase {
		id: builderTest
		name: "qrestbuilder"

		TestApi {
			id: buildApi
		}

		QtObject {
			id: buildWaiter
			property bool ok: false
			property bool done: false
			property int counter: 0

			function jcomp(actual, expected) {
				compare(actual, expected, JSON.stringify(actual) + " != " + JSON.stringify(expected));
			}

			function repConnect(reply) {
				reply.addFailedHandler(function(code, data) {
					done = true;
					testCase.fail(code + ": " + JSON.stringify(data));
				});
				reply.addErrorHandler(function(error, code, type) {
					done = true;
					testCase.fail(error + " (" + code + ", " + type + ")");
				});
			}

			function ewait(reply) {
				repConnect(reply);
				var i = 0;
				while(!done && i < 50){
					i++;
					testCase.wait(100);
				}
				done = false;
			}

			function cwait(reply, counter) {
				repConnect(reply);
				var i = 0;
				while(!done && buildWaiter.counter < counter && i < 50){
					i++;
					testCase.wait(100);
				}
				done = false;
				ok = buildWaiter.counter === counter;
				buildWaiter.counter = 0;
			}
		}

		function initTestCase() {
			verify(buildApi)
			verify(buildApi.posts)
		}

		function test_list() {
			buildWaiter.ok = false;
			var reply = buildApi.posts.listPosts();
			verify(reply);
			reply.addSucceededHandler(function(code, data){
				buildWaiter.done = true;
				compare(code, 200);
				verify(data);
				buildWaiter.ok = true;
			});
			buildWaiter.ewait(reply);
			verify(buildWaiter.ok);
		}

		function test_post() {
			buildWaiter.ok = false;
			var reply = buildApi.posts.post(42);
			verify(reply);
			reply.addSucceededHandler(function(code, data){
				buildWaiter.done = true;
				compare(code, 200);
				compare(data.id, 42);
				buildWaiter.ok = true;
			});
			buildWaiter.ewait(reply);
			verify(buildWaiter.ok);
		}
	}
}
