function Video_connecter(myid) {
	neko({ el: "#floatboxb" })
	var app = new Vue({
		el: '#videopanal',
		data: {
			mycam: "",
			opcam: "",
			time: 0,
			show: false,
			title: ""
		}
	})
	var counter = null
	var secs = 0;

	var voice = null
	var myid = chatapp._data.my_info.id + "video"

	var toid = null

	var login = false;
	var ws = null;
	var timer = null

	console.log(myid)

	ws = new WebSocket(wurl);
	var waiting = false
	var connecting = false
	/*一旦连接,就发送自己的身份数据 */
	ws.onopen = function () {
		console.log('与websocket服务器握手成功');
		ws.send(
			JSON.stringify({
				type: 'sys',
				sender: myid,
				message: {
					type: 'login',
					content: myid
				}
			})
		);
	};

	/*接收线程*/
	ws.onmessage = function (e) {
		var data = JSON.parse(e.data); //将数据本地化
		console.log(data); //打印数据便于调试
		if ((data.type == 'sys') & (data.message.type == 'logok')) {
			//如果数据是登录确认
			login = true;
			console.log('视频聊天线程: 您的视频聊天线程已经登录');
		} else if ((data.type == 'user') & (data.message.type == 'videook')) {
			voice.start()
			voice.clear()
			waiting = false
			connecting = true
			counter = setInterval(() => {
				secs++;
				app.time = "" + Math.floor(secs / 60) + ":" + secs % 60
			}, 1000)
			timer = setInterval(() => {
				var dataurl = getimgurl()
				app.mycam = dataurl
				if (login) {
					var data = {
						type: 'user',
						receiver: toid,
						sender: myid,
						message: {
							type: 'video',
							content: dataurl,
							vocontent: voice.getVoiceuUrl()
						}
					};
					ws.send(JSON.stringify(data));
					console.log('已经发送了一个数据:', data);
				}
			}, 150);
			set
		} else if ((data.type == 'user') & (data.message.type == 'videoreq')) {
			voice.start()
			if (confirm(data.sender, "请求视频通话")) {
				voice.clear()

				connecting = true
				app.show = true
				app.title = "与 " + data.sender + " 视频通话中..."
				toid = data.sender
				ws.send(JSON.stringify({
					type: 'user',
					sender: myid,
					receiver: data.sender,
					message: {
						type: 'videook',
						content: myid
					}
				}));
				counter = setInterval(() => {
					secs++;
					app.time = "" + Math.floor(secs / 60) + ":" + secs % 60
				}, 1000)
				timer = setInterval(() => {
					var dataurl = getimgurl()
					app.mycam = dataurl
					if (login) {
						var data = {
							type: 'user',
							receiver: toid,
							sender: myid,
							message: {
								type: 'video',
								content: dataurl,
								vocontent: voice.getVoiceuUrl()
							}
						};
						ws.send(JSON.stringify(data));
						console.log('已经发送了一个数据:', data);
					}
				}, 150);
			}

		} else if ((data.type == 'user') & (data.message.type == 'video')) {
			app.opcam = data.message.content;
			voice.playDataUrl(data.message.vocontent);
			console.log('视频聊天线程: 一个包播放完毕:', data);
		} else if ((data.type == 'user') & (data.message.type == 'videoclose')) {
			console.log('视频聊天线程: 对方发送了关闭命令');
			app.show = false
			if (connecting) {
				voice.stop();
				ws.send(JSON.stringify({
					type: 'user',
					sender: myid,
					receiver: toid,
					message: {
						type: 'videoclose',
						content: myid
					}
				})
				);
				connecting = false
				clearInterval(timer)
				clearInterval(counter)
				console.log('视频聊天线程: 我方发送了关闭命令');
			}

		} else {
			console.log('视频聊天线程: 收到未知的数据');
		}
	};
	voice = new Voice_tool()
	this.startwith = function (to) {
		app.show = true
		app.title = "与 " + to + " 视频通话中..."
		toid = to + "video"
		waiting = true
		var data = {
			type: 'user',
			sender: myid,
			receiver: toid,
			message: {
				type: 'videoreq',
				content: myid
			}
		}
		ws.send(JSON.stringify(data))
		console.log('视频聊天线程: 我方发送了视频请求', data);
	}
	this.close = function () {
		connecting = false
		app.show = false
		voice.stop()
		var data = {
			type: 'user',
			sender: myid,
			receiver: toid,
			message: {
				type: 'videoclose',
				content: myid
			}
		}
		ws.send(JSON.stringify(data));
		clearInterval(timer)
		clearInterval(counter)
		console.log('视频聊天线程: 我方发送了关闭命令');
	}
}