var fn = function(){
	var data = {
		posts: [],
		pages: [],
		postlets: []
	}
	for (var i = 1; i <= 100; i++) {
		data.posts.push({
							id: i,
							userId: Math.ceil(i/2),
							title: "Title" + i,
							body: "Body" + i
						})
		data.postlets.push({
							   id: i,
							   title: "Title" + i,
							   href: "/posts/" + i
						   })
	}
	for (var i = 0; i < 10; i++) {
		var page = {
			id: i,
			total: 100,
			offset: i*10,
			limit: (i+1)*10,
			next: i < 9 ? "/pages/" + (i+1) : null,
			previous: i > 0 ? "/pages/" + (i-1) : null,
			items: []
		};
		for(var j = 1; j <= 10; j++) {
			var cnt = (i*10) + j;
			page.items.push({
								id: cnt,
								userId: Math.ceil(cnt/2),
								title: "Title" + cnt,
								body: "Body" + cnt
							})
		}
		data.pages.push(page)
	}
	return data;
}
fn()
