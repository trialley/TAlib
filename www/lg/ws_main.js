"use strict";
var wurl = "ws://127.0.0.1:7999"
var voicecol = null
var videocol = null

var login = false
var ws = null


var sendtext = function (toid, text) {
	var data = {
		'type': 'user',
		'sender': chatapp._data.my_info.id,
		'receiver': toid,
		'message': {
			'type': 'text',
			'content': chatapp.text
		}
	}
	console.log("发送文字消息:", data)
	ws.send(JSON.stringify(data))
	chatapp.chatContentList.push(data)
}
var voicecall = function () {
	voicecol.startwith(chatapp._data.to_info.id)
}
var voiceclose = function () {
	voicecol.close()
}

var videocall = function () {
	videocol.startwith(chatapp._data.to_info.id)
}
var videoclose = function () {
	videocol.close()
}

var chat = function (toid) {
	console.log("调用聊天函数", toid)
	chatapp._data.to_info.id = toid
}

$(".user_item").click((e) => {
	console.log(e)
	var toid = e.currentTarget.children[1].children[0].innerHTML
	chat(toid)
})
$("#send").click(() => {
	console.log(chatapp.text)
	if (chatapp.text == "") {
		alert("输入为空")
		return 0
	}
	// $.get("./php/chatting3.php?q=" + chatapp._data.to_info.id + "&mes=" + chatapp.text, function (data, status) {

	// });

	sendtext(chatapp._data.to_info.id, chatapp.text)
	chatapp.text = ""
	var ele = document.getElementById('chatbox');
	console.log(ele.style.top, ele.scrollHeight)

	ele.style.top = -(ele.scrollHeight - 400);
})
$("#input_box").keydown(function (e) {
	if (e.shiftKey && e.keyCode == 13) {
		document.getElementById("send").click();
	}
})
