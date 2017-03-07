var fn = function(){
	var data = {
		posts: []
	};

	for (var i = 1; i <= 100; i++) {
		data.posts.push({
							id: i,
							user: {
								id: i/2,
								name: "user" + i/2
							},
							title: "Title" + i,
							body: "Body" + i
						});
	}
	return data;
}

fn();
