
function Voice_connecter(myid) {
	neko({ el: "#floatbox" })
	var app = new Vue({
		el: '#voicepanal',
		data: {
			title: "xxx",
			waitingtime: 0,
			time: 0,
			show: false
		}
	})

	var myid = chatapp._data.my_info.id + "voice"

	var toid = null
	var voice = null
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
		console.log("voice握手返回数据：", e.data)
		var data = JSON.parse(e.data); //将数据本地化
		console.log(data); //打印数据便于调试
		if ((data.type == 'sys')) {
			if ((data.message.type == 'logok')) {
				//如果数据是登录确认
				login = true;
				console.log('语音聊天线程: 您的语音聊天线程已经登录');
			}

		} else if ((data.type == 'user') & (data.message.type == 'voiceok')) {
			voice.start()
			voice.clear()

			waiting = false
			connecting = true
			var secs = 0
			setInterval(() => {
				secs++;
				app.time = "" + Math.floor(secs / 60) + ":" + secs % 60
			}, 1000)
			timer = setInterval(() => {
				var data = {
					type: 'user',
					receiver: toid,
					sender: myid,
					message: {
						type: 'voice',
						content: voice.getVoiceuUrl()
					}
				};
				ws.send(JSON.stringify(data));
				console.log('已经发送了一个数据:', data);
			}, 150);
		} else if ((data.type == 'user') & (data.message.type == 'voicereq')) {
			voice.start()
			if (confirm(data.sender, "请求语音通话")) {
				voice.clear()

				app.show = true
				app.title = "与 " + data.sender + " 语音通话..."
				toid = data.sender
				connecting = true
				ws.send(JSON.stringify({
					type: 'user',
					sender: myid,
					receiver: data.sender,
					message: {
						type: 'voiceok',
						content: myid
					}
				}));
				var secs = 0
				setInterval(() => {
					secs++;
					app.time = "" + Math.floor(secs / 60) + ":" + secs % 60
				}, 1000)
				timer = setInterval(() => {
					if (login) {
						var data = {
							type: 'user',
							receiver: toid,
							sender: myid,
							message: {
								type: 'voice',
								content: voice.getVoiceuUrl()
							}
						};
						ws.send(JSON.stringify(data));
						console.log('已经发送了一个数据:', data);
					}
				}, 150);
			}

		} else if ((data.type == 'user') & (data.message.type == 'voice')) {
			voice.playDataUrl(data.message.content);
			console.log('语音聊天线程: 一个包播放完毕:', data);
		} else if ((data.type == 'user') & (data.message.type == 'voiceclose')) {
			console.log('语音聊天线程: 对方发送了关闭命令');
			app.show = false
			if (connecting) {

				ws.send(JSON.stringify({
					type: 'user',
					sender: myid,
					receiver: toid,
					message: {
						type: 'voiceclose',
						content: myid
					}
				})
				);
				connecting = false
				clearInterval(timer)
				console.log('语音聊天线程: 我方发送了关闭命令');
			}

		} else {
			console.log('语音聊天线程: 收到未知的数据');
		}
	};

	voice = new Voice_tool()

	this.startwith = function (to) {
		app.show = true
		app.title = "与 " + to + " 语音通话中..."

		toid = to + "voice"
		waiting = true
		var data = {
			type: 'user',
			sender: myid,
			receiver: toid,
			message: {
				type: 'voicereq',
				content: myid
			}
		}
		ws.send(JSON.stringify(data))
		console.log('语音聊天线程: 我方发送了语音请求', data);
	}
	this.close = function () {
		app.show = false
		voice.stop()
		ws.send(JSON.stringify({
			type: 'user',
			sender: myid,
			receiver: toid,
			message: {
				type: 'voiceclose',
				content: myid
			}
		})
		);
		connecting = false
		clearInterval(timer)
		console.log('语音聊天线程: 我方发送了关闭命令');
	}
	//	}
}
