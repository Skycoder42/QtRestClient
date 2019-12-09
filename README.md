# QtRestClient
A library for generic JSON-based REST-APIs, with a mechanism to map JSON to Qt objects.

[![Github Actions status](https://github.com/Skycoder42/QtRestClient/workflows/CI%20build/badge.svg)](https://github.com/Skycoder42/QtRestClient/actions?query=workflow%3A%22CI%20build%22)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/5a75806bda324b14bf493c1d94ad7041)](https://www.codacy.com/app/Skycoder42/QtRestClient)
[![AUR](https://img.shields.io/aur/version/qt5-restclient.svg)](https://aur.archlinux.org/packages/qt5-restclient/)

## Features
- Consume any CBOR or JSON REST-API
- Map API objects to QObject/Q_GADGET classes. Supports:
	- basic objects
	- lists
	- paging objects
	- anything [QtJsonSerializer](https://github.com/Skycoder42/QtJsonSerializer) supports
- Allows to create API class representations to wrap specific parts of the api
- Reply-based system - use either signal/slot or a more functional approach
- Custom compiler to generate API objects, classes and methods from simple XML files
- Support for multithreading and threadpools

## Download/Installation
There are multiple ways to install the Qt module, sorted by preference:

1. Package Managers: The library is available via:
	- **Arch-Linux:** AUR-Repository: [`qt5-restclient`](https://aur.archlinux.org/packages/qt5-restclient/)
	- **MacOs:**
		- Tap: [`brew tap Skycoder42/qt-modules`](https://github.com/Skycoder42/homebrew-qt-modules)
		- Package: `qtrestclient`
		- **IMPORTANT:** Due to limitations of homebrew, you must run `source /usr/local/opt/qtrestclient/bashrc.sh` before you can use the module. Some goes for the `qtjsonserializer` dependency
2. Simply add my repository to your Qt MaintenanceTool (Image-based How-To here: [Add custom repository](https://github.com/Skycoder42/QtModules/blob/master/README.md#add-my-repositories-to-qt-maintenancetool)):
	1. Start the MaintenanceTool from the commandline using `/path/to/MaintenanceTool --addTempRepository <url>` with one of the following urls (GUI-Method is currently broken, see [QTIFW-1156](https://bugreports.qt.io/browse/QTIFW-1156)) - This must be done *every time* you start the tool:
		- On Linux: https://install.skycoder42.de/qtmodules/linux_x64
		- On Windows: https://install.skycoder42.de/qtmodules/windows_x86
		- On Mac: https://install.skycoder42.de/qtmodules/mac_x64
	2. A new entry appears under all supported Qt Versions (e.g. `Qt > Qt 5.11 > Skycoder42 Qt modules`)
	3. You can install either all of my modules, or select the one you need: `Qt Rest Client`
	4. Continue the setup and thats it! you can now use the module for all of your installed Kits for that Qt
3. Download the compiled modules from the release page. **Note:** You will have to add the correct ones yourself and may need to adjust some paths to fit your installation!
4. Build it yourself! **Note:** This requires perl to be installed. If you don't have/need cmake, you can ignore the related warnings. To automatically build and install to your Qt installation, run:
	- Install and prepare [qdep](https://github.com/Skycoder42/qdep#installation)
	- Install [QtJsonSerializer](https://github.com/Skycoder42/QtJsonSerializer#downloadinstallation)
	- Download the sources. Either use `git clone` or download from the releases. If you choose the second option, you have to manually create a folder named `.git` in the projects root directory, otherwise the build will fail.
	- `qmake`
	- `make` (If you want the tests/examples/etc. run `make all`)
	- Optional steps:
		- `make doxygen` to generate the documentation
		- `make -j1 run-tests` to build and run all tests
	- `make install`

### Building without QtJsonSerializer/qdep
If you only need the raw JSON-API, without the generic deserialization, then you can build the library without the support for those. To do so, simply run `qmake CONFIG+=no_json_serializer` instead of a parameterless qmake, and the project will be prepared without those generic APIS (it will also skip creation of qrestbuilder). This also means that the two dependencies, qdep and QtJsonSerializer, are *not required* anymore and do not have to be installed (you can ignore related warnings).

Also, when building the library in this configuration, do not run `make all`, as the tests will fail to build against this configuration. Simply run `make` instead. To build the examples, run `make sub-examples`.

**Important:** Please note that you must not include `#include <QtRestClient>` when using the library in this configuration. Instead, include the needed headers directly, e.g. `#include <QtRestClient/RestClient>`.

## Usage
The restclient is provided as a Qt module. Thus, all you have to do is add the module, and then, in your project, add `QT += restclient` to your `.pro` file!

The API consists of 3 main classes:

- **RestClient:** Set up the API, i.e. how to access it, the base URL and headers
- **RestClass:** A subset of the API, allows to make requests
- **RestReply/GenericRestReply:** The reply control returned for every request, to obtain the result and react on errors

### Example
The following example shows an example request made to [JSONPlaceholder](https://jsonplaceholder.typicode.com/).

#### API-Definition
Before we can start using the API, we have to define c++ objects to map the JSON to. For this example, all we need is to represent a post. This example makes use of Q_GADGET, but it works the same for QObjets as well. MEMBER properties are used for simplicity, it works with any normal property. See [QtJsonSerializer](https://github.com/Skycoder42/QJsonSerializer) for more details about the serialization part. **Note:** This is optional. You can in fact use the API with JSON only.
```cpp
struct Post
{
	Q_GADGET

	Q_PROPERTY(int id MEMBER id)
	Q_PROPERTY(int userId MEMBER userId)
	Q_PROPERTY(QString title MEMBER title)
	Q_PROPERTY(QString body MEMBER body)

public:
	Post();

	int id;
	int userId;
	QString title;
	QString body;
};
```

#### API-Use
We will now query the data of the post with the id 42. The final URL is: https://jsonplaceholder.typicode.com/posts/42

First, you set up the client, for example in your main:
```cpp
auto client = new QtRestClient::RestClient();
client->setBaseUrl(QUrl("https://jsonplaceholder.typicode.com"));
QtRestClient::addGlobalApi("jsonplaceholder", client);//optional, makes it easier to use the api
```

Now, in any class you want to use the api, you can create rest classes to do so:
```cpp
auto restClass = QtRestClient::createApiClass("jsonplaceholder", "posts", this);//creates a restclass for the registered API, with the sub-path "posts"
```

Finally, we send the get-request and log the reply:
```cpp
restClass->get<Post>("42")->onSucceeded([](int statusCode, Post data) {  //calls: GET https://jsonplaceholder.typicode.com/posts/42
	//do whatever you want with the data
	qDebug() << data.id
			 << data.userId
			 << data.title
			 << data.body
});
```

And thats all you need for a basic API access. Check the documentation for more and *important* details about the client, the class and the replies!

### Multithreading
Generally speaking, the library primarily supports single threaded execution, as all requests and replies are handled asynchronously thanks to signals and slots. For most applications, you should keep it that way, as multithreading will only slow down your application, as long as your workload is not big enough to justify threaded mode.

Please note that it is easily possible to create multiple different instances of the RestClient in their own respective thread. However, if you want to use the **same** instance in multiple threads, make sure to read the following carefully.

#### Simple threading
To enable basic threading support, simply set the `QtRestClient::RestClient::threaded` property to true. This will make all members of the client threadsafe, as well as of any `QtRestClient::RestClass` belonging to it. From that
point on, any request sent via this client will have the requests asynchronously posted to it's original thread.
This means requests are still *sent* from the primary thread, but any steps before that (i.e. creation, serialization, encoding) are run on whatever thread you call the method to send.

The `QtRestClient::RestReply` classes handle this case transparently, you can use them the same as for normal single threaded operation. The only difference is, that their reply member may be `nullptr` for a while after
sending the request. The handlers assigned to the replies (e.g. `onSuccess(...)`) will be executed on the **same thread** as the reply was created on.

For this all to work, *all* threads that you use any class or reply on, *must* be a QThread with a running **eventloop** (QThread::exec). Otherwise the replies will not be notified of the completition and never run any of the handlers.

#### Threadpooling replies
In addition to this general multithreading support, it is also possible to let `QtRestClient::RestReply` instances run on a QThreadPool. To do so, you can either call `QtRestClient::RestReply::makeAsync` or set set `QtRestClient::RestClient::asyncPool` property - the latter affecting all replies created for a certain client.

The network I/O will still be performed by the main thread, but the parsing, deserialization and proccessing via the handlers is run on an arbitrary thread of the pool. This mode should only be used in combination with the threaded mode.

As threadpools do not run a standard eventloop, any request sent from within a pooled handler **must** call `makeAsync` on the replies to make sure they are correctly handled. Otherwise they replies assume they are on a normal QThread and will *never* complete. You can skip this step, if `QtRestClient::RestClient::asyncPool` was set, as this internally makes any reply send from the clients classes asynchronous.

You can use the pooling without multithreading enabled, but in that case you **must not** send new requests from a handler. This means `QtRestClient::Paging` and `QtRestClient::Simple` objects cannot be used.

### API-Generator
The library comes with a tool to create API data classes and wrapper classes over the rest client. Those are generated from XML files and allow an easy creation of APIs in your application. The tool is build as a custom compiler and added to qmake. To use the tool simply add the json files to you pro file, and the sources will be automatically generated and compiled into your application!

#### Example
The following example shows a XML file to generate the post type from above (with shared data optimizations, automatic registration, etc.). It could be named `post.xml`:
```xml
<RestGadget name="Post">
	<Property key="id" type="int">-1</Property>
	<Property key="userId" type="int">-1</Property>
	<Property key="title" type="QString"/>
	<Property key="body" type="QString"/>
</RestGadget>
```

A definition for a very simple API, that allows some simple operations for posts, could look like this. It could be named `api.xml`:
```xml
<RestApi name="ExampleApi" globalName="jsonplaceholder">
	<Include local="true">api_posts.h</Include>
	<BaseUrl>https://jsonplaceholder.typicode.com</BaseUrl>

	<Class key="posts" type="PostClass"/>
</RestApi>
```

You can either directly add the methods to the API, or, like I did in this example, split the classes into different files. It could be named `api_posts.xml`:
```xml
<RestClass name="PostClass">
	<Include local="true">post.h</Include>
	<Path>posts</Path>

	<Method name="listPosts" verb="GET" returns="QList&lt;Post&gt;"/>
	<Method name="post" returns="Post">
		<PathParam key="id" type="int"/>
	</Method>
	<Method name="savePost" verb="POST" body="Post" returns="Post"/>
	<Method name="updatePost" verb="PUT" body="Post" returns="Post">
		<PathParam key="id" type="int"/>
	</Method>
	<Method name="deletePost" verb="DELETE">
		<PathParam key="id" type="int"/>
	</Method>
</RestClass>
```

These files are added to the `.pro` file as follows:
```pro
QT += restclient

REST_API_FILES += post.xml \
	api.xml \
	api_posts.xml
```

And thats it! once you run qmake and compile your application, those APIs will be generated for you. The usage is fairly simple:
```cpp
// by using the API directly:
auto api = new ExampleApi(this);
auto reply = api->posts()->post(42);
// continue as usual with any generic network reply

// by using the factory to create the class only:
auto posts = ExampleApi::factory().posts().instance(this);
auto reply = posts->post(42);
```

Check the JsonPlaceholderApi example in `examples/restclient` for this example.

### Authentication
When I was trying to implement authentication, I found that it can be quite heterogenous. Because of that, I did not implement a specific authentication mechanism. Instead, you can either use one supported by Qt or create your own auth flow. The easiest way to do so is to create a RestClient, and then perform whatever authentication you need with the clients internal QNetworkAccessManager, and set parameters, headers, cookies, etc accordingly, if neccessary.

Qt itself supports the following kinds of authentication:

- [**QAuthenticator:**](https://doc.qt.io/qt-5/qauthenticator.html) Integrated in QNetworkAccessManager, via a signal. See Qt Docs for supported mechanisms
- [**QtNetworkAuth:**](https://doc.qt.io/qt-5/qtnetworkauth-index.html) A Technology preview providing OAuth and OAuth2 authentication flows.

**Note:** For the latter, use the `restclientauth` module. The following examples explains how

#### Example
```
QAbstractOAuth *oAuth = ...; // create an OAuth instance as needed
// perform the usual handshake to setup the oAuth object
oAuth->grant();
// ...

// create a client from the oAuth
auto client = new QtRestClient::Auth::AuthRestClient(oAuth);
// continue as per usual
```

### QObject Ownership
If you are using QtRestClient with QObjects, please be aware that none of the RestClients functions take ownership of the returend objects. This means **you** are responsible for deleting them, if not needed anymore. In short, you as caller own the objects returned to your handlers. If you are uncertain whether you need to handle an object or not, check the documentation of the specific function for details.

## Documentation
The documentation is available on [github pages](https://skycoder42.github.io/QtRestClient/). It was created using [doxygen](http://www.doxygen.org/). The HTML-documentation and Qt-Help files are shipped
together with the module for both the custom repository and the package on the release page. Please note that doxygen docs do not perfectly integrate with QtCreator/QtAssistant.
