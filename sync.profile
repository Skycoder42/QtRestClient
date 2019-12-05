%modules = (
	"QtRestClient" => "$basedir/src/restclient",
	"QtRestClientAuth" => "$basedir/src/restclientauth",
);

$publicclassregexp = "QtRestClient::(?!PagingData|__private).+";

%classnames = (
	"simple.h" => "Simple",
);
