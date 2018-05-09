import QtQuick 2.5
import de.skycoder42.RestClient 1.3
import QtTest 1.1

Item {
	id: root

	TestCase {
		name: "RestClient"

		RestClient {
			id: api

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
		}
	}
}
