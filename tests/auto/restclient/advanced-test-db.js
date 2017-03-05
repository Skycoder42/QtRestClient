var fn = function(){
	var data = {
		posts: [],
		pages: [],
		postlets: [],
		pagelets: []
	};

	for (var i = 1; i <= 100; i++) {
		//posts
		data.posts.push({
							id: i,
							userId: Math.ceil(i/2),
							title: "Title" + i,
							body: "Body" + i
						});

		//postlets
		data.postlets.push({
							   id: i,
							   title: "Title" + i,
							   href: "/posts/" + i
						   });
	}

	for (i = 0; i < 10; i++) {
		//pages
		var page = {
			id: i,
			total: 100,
			offset: i*10,
			next: i < 9 ? "/pages/" + (i+1) : null,
			previous: i > 0 ? "/pages/" + (i-1) : null,
			items: []
		};
		for(var j = 0; j < 10; j++)
			page.items.push(data.posts[(i*10) + j]);
		data.pages.push(page);

		//pagelets
		var pagelet = {
			id: i,
			next: i < 9 ? "/pagelets/" + (i+1) : null,
			items: []
		};
		for(j = 0; j < 10; j++)
			pagelet.items.push(data.postlets[(i*10) + j]);
		data.pagelets.push(pagelet);
	}

	return data;
}

fn();
