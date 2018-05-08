# QtRestClient
A library for generic JSON-based REST-APIs, with a mechanism to map JSON to Qt objects.

[![Travis Build Status](https://travis-ci.org/Skycoder42/QtRestClient.svg?branch=master)](https://travis-ci.org/Skycoder42/QtRestClient)
[![Appveyor Build status](https://ci.appveyor.com/api/projects/status/66vntcoho3t4x1jw/branch/master?svg=true)](https://ci.appveyor.com/project/Skycoder42/qtrestclient/branch/master)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/5a75806bda324b14bf493c1d94ad7041)](https://www.codacy.com/app/Skycoder42/QtRestClient)
[![AUR](https://img.shields.io/aur/version/qt5-restclient.svg)](https://aur.archlinux.org/packages/qt5-restclient/)

## Features
- Consume any JSON-REST-API
- Map API objects to QObject/Q_GADGET classes. Supports:
	- basic objects
	- lists
	- paging objects
- Allows to create API class representations to wrap specific parts of the api
- Reply-based system - use either signal/slot or a more functional approach
- Custom compiler to generate API objects, classes and methods from simple XML files

## Download/Installation
There are multiple ways to install the Qt module, sorted by preference:

1. Package Managers: The library is available via:
	- **Arch-Linux:** AUR-Repository: [`qt5-restclient`](https://aur.archlinux.org/packages/qt5-restclient/)
	- **Ubuntu:** Launchpad-PPA: [ppa:skycoder42/qt-modules](https://launchpad.net/~skycoder42/+archive/ubuntu/qt-modules), package `libqt5restclient[1/-dev]`
	- **MacOs:**
		- Tap: [`brew tap Skycoder42/qt-modules`](https://github.com/Skycoder42/homebrew-qt-modules)
		- Package: `qtrestclient`
		- **IMPORTANT:** Due to limitations of homebrew, you must run `source /usr/local/opt/qtrestclient/bashrc.sh` before you can use the module. Some goes for the `qtjsonserializer` dependency
2. Simply add my repository to your Qt MaintenanceTool (Image-based How-To here: [Add custom repository](https://github.com/Skycoder42/QtModules/blob/master/README.md#add-my-repositories-to-qt-maintenancetool)):
	1. Open the MaintenanceTool, located in your Qt install directory (e.g. `~/Qt/MaintenanceTool`)
	2. Select `Add or remove components` and click on the `Settings` button
	3. Go to `Repositories`, scroll to the bottom, select `User defined repositories` and press `Add`
	4. In the right column (selected by default), type:
		- On Linux: https://install.skycoder42.de/qtmodules/linux_x64
		- On Windows: https://install.skycoder42.de/qtmodules/windows_x86
		- On Mac: https://install.skycoder42.de/qtmodules/mac_x64
	5. Press `Ok`, make shure `Add or remove components` is still selected, and continue the install (`Next >`)
	6. A new entry appears under all supported Qt Versions (e.g. `Qt > Qt 5.10.1 > Skycoder42 Qt modules`)
	7. You can install either all of my modules, or select the one you need: `Qt Rest Client`
	8. Continue the setup and thats it! you can now use the module for all of your installed Kits for that Qt Version
3. Download the compiled modules from the release page. **Note:** You will have to add the correct ones yourself and may need to adjust some paths to fit your installation!
4. Build it yourself! **Note:** This requires perl to be installed. If you don't have/need cmake, you can ignore the related warnings. To automatically build and install to your Qt installation, run:
	- `qmake`
	- `make qmake_all`
	- `make` (If you want the tests/examples/etc. run `make all`)
	- Optional steps:
		- `make doxygen` to generate the documentation
		- `make lrelease` to generate the translations
	- `make install`

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
restClass->get<Post>("42")->onSucceeded([](RestReply *reply, int statusCode, Post data) { //calls: GET https://jsonplaceholder.typicode.com/posts/42
	//do whatever you want with the data
	qDebug() << data.id
			 << data.userId
			 << data.title
			 << data.body
});
```

And thats all you need for a basic API access. Check the documentation for more and *important* details about the client, the class and the replies!

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
//by using the API directly:
auto api = new ExampleApi(this);
auto reply = api->posts()->post(42);
//continue as usual with any generic network reply

//by using the factory to create the class only:
auto posts = ExampleApi::factory().posts().instance(this);
auto reply = posts->post(42);
```

Check the JsonPlaceholderApi example in `examples/restclient` for this example.

### Authentication
When I was trying to implement authentication, I found that it can be quite heterogenous. Because of that, I did not implement a specific authentication mechanism. Instead, you can either use one supported by Qt or create your own auth flow. The easiest way to do so is to create a RestClient, and then perform whatever authentication you need with the clients internal QNetworkAccessManager, and set parameters, headers, cookies, etc accordingly, if neccessary.

Qt itself supports the following kinds of authentication:
- [**QAuthenticator:**](https://doc.qt.io/qt-5/qauthenticator.html) Integrated in QNetworkAccessManager, via a signal. See Qt Docs for supported mechanisms
- [**QtNetworkAuth:**](https://doc.qt.io/qt-5/qtnetworkauth-index.html) A Technology preview providing OAuth and OAuth2 authentication flows.

**Note:** For the latter, I may add support in a Future version. For the first one, simply connect to the managers signal.

### QObject Ownership
If you are using QtRestClient with QObjects, please be aware that none of the RestClients functions take ownership of the returend objects. This means **you** are responsible for deleting them, if not needed anymore. In short, you as caller own the objects returned to your handlers. If you are uncertain whether you need to handle an object or not, check the documentation of the specific function for details.

## Documentation
The documentation is available on [github pages](https://skycoder42.github.io/QtRestClient/). It was created using [doxygen](http://www.doxygen.org/). The HTML-documentation and Qt-Help files are shipped
together with the module for both the custom repository and the package on the release page. Please note that doxygen docs do not perfectly integrate with QtCreator/QtAssistant.
