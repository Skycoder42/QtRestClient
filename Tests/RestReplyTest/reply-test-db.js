var fn = function(){
	var data = {
		posts: [],
		pages: []
	}
	for (var i = 1; i <= 100; i++) {
		data.posts.push({
							id: i,
							userId: Math.ceil(i/2),
							title: "Title" + i,
							body: "Body" + i
						})
	}
	for (var i = 0; i < 100; i++) {
		var page = {
			total: 100,
			offset: i,
			limit: Math.min(i+10, 100),
			next: i < 90 ? "/pages/" + (i+10) : null,
			previous: i >= 10 ? "/pages/" + (i-10) : null,
			items: []
		};
		for(var j = 1; j <= 10; j++) {
			var cnt = i + j;
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
