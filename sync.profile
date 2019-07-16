%modules = (
	"QtRestClient" => "$basedir/src/restclient",
	"QtRestClientAuth" => "$basedir/src/restclientauth",
);

$publicclassregexp = "QtRestClient::(?!PagingData|MetaComponent).+"
