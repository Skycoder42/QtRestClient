import QtQuick 2.5
import de.skycoder42.RestClient 1.3
import QtTest 1.1

Item {
	id: root

	//TODO make full testcase via http server
	TestCase {
		name: "RestClient"

		RestClient {
			id: api

			Component.onCompleted: QtRestClient.addGlobalApi("testapi", api)

			RestClass {
				id: postClass
				path: "post"
			}

			RestClass {
				id: userClass
				path: "user"

				RestClass {
					id: userNameClass
					path: "name"
				}
			}
		}

		function test_valid() {
			// check if all restclasses have been initialized
			verify(postClass.restClass);
			verify(userClass.restClass);
			verify(userNameClass.restClass);

			verify(QtRestClient.apiClient("testapi"));
			verify(QtRestClient.createApiClass("testapi", "some/sub/path", root));

			verify(postClass.get());
			verify(userClass.put(["hello", "world"]));
			verify(userNameClass.post({
										  id: 42,
										  name: "user"
									  }));

			postClass.get().completed.connect(function(code, data) {
				console.log(code);
				var paging = QtRestClient.createPaging(api, data);
				verify(paging);
				paging.iterate(function(item, index){
					console.log(index, item);
				});
			});
		}
	}
}
