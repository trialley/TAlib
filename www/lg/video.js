var video = document.createElement('video');
var canvas = document.createElement('canvas');
// video.style.cssText = "width:70px;height:125px"
// canvas.style.cssText = "width:70px;height:125px"
var promise = navigator.mediaDevices.getUserMedia({
	video: {width:80,height:140},
	audio: false
});
promise.then(function(MediaStream) {
	video.srcObject = MediaStream;
	video.play();
});
var ctx = canvas.getContext('2d');

var getimgurl = function() {
	ctx.drawImage(video, 0, 0, 105, 57);
	return canvas.toDataURL('image/png');
}