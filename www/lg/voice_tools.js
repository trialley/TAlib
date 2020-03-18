var Voice_tool = function () {

	var SRecorder = function (stream) {
		var config = {};

		config.sampleBits = config.smapleBits || 8; //输出采样位数
		config.sampleRate = config.sampleRate || (44100 / 6); //输出采样频率

		var context = new AudioContext();
		var audioInput = context.createMediaStreamSource(stream);
		var recorder = context.createScriptProcessor(4096, 1, 1); //录音缓冲区大小，输入通道数，输出通道数

		var audioData = {
			size: 0 //录音文件长度
			,
			buffer: [] //录音缓存
			,
			inputSampleRate: context.sampleRate //输入采样率
			,
			inputSampleBits: 16 //输入采样数位 8, 16
			,
			outputSampleRate: config.sampleRate //输出采样率
			,
			oututSampleBits: config.sampleBits //输出采样数位 8, 16
			,
			clear: function () {
				this.buffer = [];
				this.size = 0;
			},
			input: function (data) {
				this.buffer.push(new Float32Array(data));
				this.size += data.length;
			},
			compress: function () { //合并压缩
				//合并
				var data = new Float32Array(this.size);
				var offset = 0;
				for (var i = 0; i < this.buffer.length; i++) {
					data.set(this.buffer[i], offset);
					offset += this.buffer[i].length;
				}
				//压缩
				var compression = parseInt(this.inputSampleRate / this.outputSampleRate);
				var length = data.length / compression;
				var result = new Float32Array(length);
				var index = 0,
					j = 0;
				while (index < length) {
					result[index] = data[j];
					j += compression;
					index++;
				}
				return result;
			},
			encodeWAV: function () {
				var sampleRate = Math.min(this.inputSampleRate, this.outputSampleRate);
				var sampleBits = Math.min(this.inputSampleBits, this.oututSampleBits);
				var bytes = this.compress();
				var dataLength = bytes.length * (sampleBits / 8);
				var buffer = new ArrayBuffer(44 + dataLength);
				var data = new DataView(buffer);

				var channelCount = 1; //单声道
				var offset = 0;

				var writeString = function (str) {
					for (var i = 0; i < str.length; i++) {
						data.setUint8(offset + i, str.charCodeAt(i));
					}
				};

				// 资源交换文件标识符
				writeString('RIFF');
				offset += 4;
				// 下个地址开始到文件尾总字节数,即文件大小-8
				data.setUint32(offset, 36 + dataLength, true);
				offset += 4;
				// WAV文件标志
				writeString('WAVE');
				offset += 4;
				// 波形格式标志
				writeString('fmt ');
				offset += 4;
				// 过滤字节,一般为 0x10 = 16
				data.setUint32(offset, 16, true);
				offset += 4;
				// 格式类别 (PCM形式采样数据)
				data.setUint16(offset, 1, true);
				offset += 2;
				// 通道数
				data.setUint16(offset, channelCount, true);
				offset += 2;
				// 采样率,每秒样本数,表示每个通道的播放速度
				data.setUint32(offset, sampleRate, true);
				offset += 4;
				// 波形数据传输率 (每秒平均字节数) 单声道×每秒数据位数×每样本数据位/8
				data.setUint32(offset, channelCount * sampleRate * (sampleBits / 8), true);
				offset += 4;
				// 快数据调整数 采样一次占用字节数 单声道×每样本的数据位数/8
				data.setUint16(offset, channelCount * (sampleBits / 8), true);
				offset += 2;
				// 每样本数据位数
				data.setUint16(offset, sampleBits, true);
				offset += 2;
				// 数据标识符
				writeString('data');
				offset += 4;
				// 采样数据总数,即数据总大小-44
				data.setUint32(offset, dataLength, true);
				offset += 4;
				// 写入采样数据
				if (sampleBits === 8) {
					for (var i = 0; i < bytes.length; i++ , offset++) {
						var s = Math.max(-1, Math.min(1, bytes[i]));
						var val = s < 0 ? s * 0x8000 : s * 0x7FFF;
						val = parseInt(255 / (65535 / (val + 32768)));
						data.setInt8(offset, val, true);
					}
				} else {
					for (var i = 0; i < bytes.length; i++ , offset += 2) {
						var s = Math.max(-1, Math.min(1, bytes[i]));
						data.setInt16(offset, s < 0 ? s * 0x8000 : s * 0x7FFF, true);
					}
				}

				return new Blob([data], {
					type: 'audio/wav'
				});
			}
		};

		this.start = function () {
			audioInput.connect(recorder);
			recorder.connect(context.destination);
		}

		this.stop = function () {
			recorder.disconnect();
		}

		this.getBlob = function () {
			return audioData.encodeWAV();
		}

		this.clear = function () {
			audioData.clear();
		}

		recorder.onaudioprocess = function (e) {
			audioData.input(e.inputBuffer.getChannelData(0));
		}

	};
	SRecorder.get = function (callback) {
		if (callback) {
			if (navigator.getUserMedia) {
				navigator.getUserMedia({
					audio: true
				},
					function (stream) {
						var rec = new SRecorder(stream);
						callback(rec);
					},
					function (e) {
						console.error(e)
					})
			}
		}
	}


	var gRecorder = null
	SRecorder.get(function (rec) {
		console.log("初始化grecorder");
		gRecorder = rec;
	})
	var recordreader = new FileReader(); //初始化播放解码器

	var dataURLtoBlob = function (dataurl) {
		var arr = dataurl.split(','),
			mime = arr[0].match(/:(.*?);/)[1],
			bstr = atob(arr[1]),
			n = bstr.length,
			u8arr = new Uint8Array(n);
		while (n--) {
			u8arr[n] = bstr.charCodeAt(n);
		}
		return new Blob([u8arr], {
			type: mime
		});
	}
	var audioContext = new AudioContext() //初始化播放器
	var queue = []
	var sendqueue = []

	var playreader = new FileReader(); //初始化播放解码器

	
var timer=null
	//return function () {
	var gstart=false
		this.start = function () {
			console.log("启动voice");
			if(!gstart){
				gRecorder.start();
			}
			gRecorder.clear();
			setTimeout(() => {
				timer=setInterval(() => {
					var blob = gRecorder.getBlob()
					gRecorder.clear();
					recordreader.readAsDataURL(blob);
					recordreader.onload = function (e) {
						sendqueue.push(recordreader.result);
					}
				}, 150)
			}, 1000)
		}
		this.getVoiceuUrl = function () {
			return sendqueue.shift()
		}
		this.stop = function () {
			console.log("停止voice");
			clearInterval(timer)
			//gRecorder.stop();
		}
		this.clear=function(){
			gRecorder.clear();
		}


		this.playDataUrl = function (content) {
			playreader.readAsArrayBuffer(dataURLtoBlob(content));
			playreader.onload = function (e) {
				audioContext.decodeAudioData(this.result, function (buffer) {
					queue.push(buffer)
				}, function (e) {
					console.log("failed to decode the file");
				});
			}

			console.log("接收到新包,待播放排队数:", queue.length)

			var audioBufferSouceNode = audioContext.createBufferSource();
			audioBufferSouceNode.buffer = queue.shift();
			audioBufferSouceNode.connect(audioContext.destination);
			audioBufferSouceNode.start();
			console.log("一个包播放完毕:", audioContext.state, queue.length);
		}
	//}
}