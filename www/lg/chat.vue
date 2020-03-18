<template>
	<view>
		<view class="content" @touchstart="hideDrawer">
			<scroll-view
				class="msg-list"
				scroll-y="true"
				:scroll-with-animation="scrollAnimation"
				:scroll-top="scrollTop"
				:scroll-into-view="scrollToView"
				@scrolltoupper="loadHistory"
				upper-threshold="50"
			>
				<view class="row" 
				v-for="(row, index) in msgList" 
				:key="index" 
				:index="index"
				>
					<!-- 系统消息 -->
					<block v-if="row.type == 'sys'">
						<view class="system">
							<!-- 文字消息 -->
							<view class="text">{{ row.message.type }}</view>
						</view>
					</block>

					<!-- 用户消息 -->
					<block v-if="row.type == 'user'">
						<!-- 自己发出的消息 -->
						<view class="my" v-if="row.sender== myid">
							<!-- 我的消息 -->
							<view class="left">
								<!-- 文字消息 -->
								<view v-if="row.message.type == 'text'" class="bubble">
									<rich-text :nodes="row.message.content"></rich-text>
								</view>
								<!-- 语言消息 -->
								<view
								v-if="row.message.type == 'voice'" 
								class="bubble voice" 
								@tap="playVoice(row.message.content)"
								>
									<view class="length">{{ row.message.type }}</view>
									<view class="icon my-voice"></view>
								</view>
								<!-- 图片消息 -->
								<view v-if="row.sender== 'img'" 
								class="bubble img" 
								@tap="showPic(row.message.content)">
									<image :src="row.message.content" :style="{}"></image>
								</view>
							</view>
							<!-- 我的头像 -->
							<view class="right"><image :src="1"></image></view>
						</view>
						<!-- 别人发出的消息 -->
						<view class="other" v-if="row.sender == toid">
							<!-- 对方头像 -->
							<view class="left"><image :src="1"></image></view>
							<!-- 对方消息 -->
							<view class="right">
								<view class="username">
									<view class="name">{{ row.sender }}</view>
									<view class="time"></view>
								</view>
								<!-- 文字消息 -->
								<view v-if="row.message.type == 'text'" 
								class="bubble"><rich-text :nodes="row.message.content"></rich-text></view>
								<!-- 语音消息 -->
								<view v-if="row.message.type == 'voice'" 
								class="bubble voice" 
								@tap="playVoice(row.message.content)" 
								>
									<view class="icon other-voice"></view>
									<view class="length">{{ row.message.type }}</view>
								</view>
								<!-- 图片消息 -->
								<view
								v-if="row.message.type == 'img'" 
								class="bubble img" 
								@tap="showPic(row.message.content)">
									<image :src="row.message.content" :style="{}"></image>
								</view>
							</view>
						</view>
					</block>
				</view>
			</scroll-view>
		</view>

		<!-- 抽屉栏 -->
		<view class="popup-layer" :class="popupLayerClass">
			<!-- 更多功能-->
			<view class="more-layer" :class="{ hidden: hideMore }">
				<view class="list">
					<view class="box" @tap="chooseImage"><view class="icon tupian2"></view></view>
					<view class="box cuIcon-phone" @tap="voicecallt1"></view>
					<view class="box cuIcon-phone" @tap="voicecallt2"></view>
					<view class="box cuIcon-record" @tap="vediocallt1"></view>
					<view class="box cuIcon-record" @tap="vediocallt2"></view>
				</view>
			</view>
		</view>

		<!-- 底部输入栏 -->
		<view class="input-box" :class="popupLayerClass">
			<view class="voice"><view class="icon yuyin"></view></view>
			<view class="textbox">
				<view class="text-mode">
					<view class="box"><textarea auto-height="true" v-model="textMsg" /></view>
				</view>
			</view>
			<view class="more" @tap="showMore"><view class="icon add"></view></view>
			<view class="send" @tap="sendText"><view class="btn">发送</view></view>
		</view>
	</view>
</template>
<script>
export default {
	data() {
		return {
			//文字消息
			textMsg: '',
			//消息列表
			scrollAnimation: false,
			scrollTop: 0,
			scrollToView: '',
			msgList: [],
			msgImgList: [],

			//播放语音相关参数
			AUDIO: uni.createInnerAudioContext(),
			playMsgid: null,
			VoiceTimer: null,

			// 抽屉参数
			popupLayerClass: '',
			// more参数
			hideMore: true,
			windowsState: '',
			toid:null,
			myid:null,
			ws:null
		};
	},
	onLoad(option) {
		console.log(this);
		this.toid=option.toid
		this.myid=option.myid
		this.getLocalMsg();
		uni.setNavigationBarTitle({
			title: this.toid
		});
		//语音自然播放结束
		this.AUDIO.onEnded(res => {
			this.playMsgid = null;
		});
	},
	methods: {
		voicecallt1(){
			console.log("点击语音通话21")
			uni.navigateTo({
				//url:"/pages/chatpage/voice?toid="+this.toid+"&myid="+this.myid
				url:"/pages/chatpage/voice?toid=1voice&myid=2voice"
			})
		},
		voicecallt2(){
			console.log("点击语音通话12")
			uni.navigateTo({
				//url:"/pages/chatpage/voice?toid="+this.toid+"&myid="+this.myid
				url:"/pages/chatpage/voice?toid=2voice&myid=1voice"
			})
		},
		vediocallt2(){
			console.log("点击视频通话12")
			uni.navigateTo({
				//url:"/pages/chatpage/voice?toid="+this.toid+"&myid="+this.myid
				url:"/pages/chatpage/vedio?toid=2vedio&myid=1vedio"
			})
		},
		vediocallt1(){
			console.log("点击视频通话21")
			uni.navigateTo({
				//url:"/pages/chatpage/voice?toid="+this.toid+"&myid="+this.myid
				url:"/pages/chatpage/vedio?toid=1vedio&myid=2vedio"
			})
		},
		
		// 加载初始页面消息
		getLocalMsg(myid) {
			//var myinfo = uni.getStorageSync(myid+toid);
			var myinfo =[
				{
					'type':'sys',
					'sender':'sys',
					'message':{
						'type':'conok',
						'content':'聊天建立成功'
					}
				},{
					'type':'user',
					'sender':'222',
					'receiver': '111',
					'message':{
						'type':'text',
						'content':'恭喜发财!'
					}
				},{
					'type':'user',
					'sender':'111',
					'receiver': '222',
					'message':{
						'type':'text',
						'content':'谢谢您!'
					}
				},{
					'type':'user',
					'sender':'111',
					'receiver': '222',
					'message':{
						'type':'text',
						'content':'谢谢您!'
					}
				}
			]
			this.msgList=myinfo
			// 滚动到底部
			this.$nextTick(function() {
				//进入页面滚动到底部
				this.scrollTop = 9999;
				this.$nextTick(function() {
					this.scrollAnimation = true;
				});
			});
		},

		//更多功能(点击+弹出)
		showMore() {
			if (this.hideMore) {
				this.hideMore = false;
				this.popupLayerClass = 'showLayer';
			} else {
				this.popupLayerClass = '';
				this.hideMore = true;
			}
		},

		// 选择图片发送
		chooseImage() {
			uni.chooseImage({
				sourceType: ['album'],
				sizeType: ['original', 'compressed'], //可以指定是原图还是压缩图，默认二者都有
				success: res => {
					for (let i = 0; i < res.tempFilePaths.length; i++) {
						uni.getImageInfo({
							src: res.tempFilePaths[i],
							success: image => {
								console.log(image.width);
								console.log(image.height);
								let msg = { url: res.tempFilePaths[i], w: image.width, h: image.height };
								this.sendMsg(msg, 'img');
							}
						});
					}
				}
			});
		},

		// 发送文字消息
		sendText() {
			if (!this.textMsg) {
				return;
			}
			let content = this.textMsg;
			this.wssendMsg(content, 'text');
			this.textMsg = ''; //清空输入框
		},
		// 发送消息
		wssendMsg(content, type) {
			let msg = {
					'type':'user',
					'sender':this.myid,
					'receiver': this.toid,
					'message':{
						'type':type,
						'content':content
					}
			};

			this.msgList.push(msg);
			ws.send(JSON.stringify(msg));
		},

		// 添加文字消息到列表
		addTextMsg(msg) {
			this.msgList.push(msg);
		},
		// 添加语音消息到列表
		addVoiceMsg(msg) {
			this.msgList.push(msg);
		},
		// 添加图片消息到列表
		addImgMsg(msg) {
			msg.msg.content = this.setPicSize(msg.msg.content);
			this.msgImgList.push(msg.msg.content.url);
			this.msgList.push(msg);
		},

		// 添加系统文字消息到列表
		addSystemTextMsg(msg) {
			this.msgList.push(msg);
		},
		// 预览图片
		showPic(msg) {
			uni.previewImage({
				indicator: 'none',
				current: msg.content.url,
				urls: this.msgImgList
			});
		},
		// 播放语音
		playVoice(msg) {
			this.playMsgid = msg.id;
			this.AUDIO.src = msg.content.url;
			this.$nextTick(function() {
				this.AUDIO.play();
			});
		}
	}
};
</script>
<style lang="scss">
@import '@/static/HM-chat/css/style.scss';
</style>
