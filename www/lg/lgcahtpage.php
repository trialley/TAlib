<?php
	include"./php/shield.php";  //防御恶意访问的函数在这个文件里面
							//该引用务必在<html>标签之前.
?>
<html>
<!-- 还是先拿到自己的信息 -->
<?php
		session_start();
		$userid = $_SESSION['userid'];
		$nickname =     $_SESSION['nickname'];
		$photourl =     $_SESSION['photourl'];
		$mailaddr =     $_SESSION['mailaddr'];
		$sex =     $_SESSION['sex'];
		$age =     $_SESSION['age'];
		$selfintro =     $_SESSION['selfintro'];
		$password =     $_SESSION['password'];
		//echo "<script>alert('".$photourl."');</script>";
		//echo "<script>alert('".$photourl."');</script>";
		//echo "<script>alert('". 'src="'.$photourl.'"' . "');</script>";
?>
<?php
		$dbservername = "39.96.9.85";
		$dbusername = "root";
		$dbpassword = "db";
		$dbname = "Link";
		// 创建连接
		$conn = mysqli_connect($dbservername, $dbusername, $dbpassword, $dbname);
		// Check connection
		if (!$conn) {
				die("数据库连接失败,可能是服务器宕机,请您联系管理员:" . mysqli_connect_error());
		}
		$program_char = "utf8" ;
		mysqli_set_charset( $conn , $program_char );

		$sql = "SELECT * FROM chatList WHERE userID = ".$userid;

		$chatlist = mysqli_query($conn, $sql);
		$num = mysqli_num_rows($chatlist);
		//chatlist现在存着userID和friendID
		//num当前为chat数量，用于控制接下来的for循环
?>





<head>
<meta charset="utf-8">
<title>Circle_GS_chatPage</title>
<link rel="stylesheet" href="css/amazeui.min.css" />
<!-- <link rel="stylesheet" href="css/main.css" /> -->
<?php include_once "./css/main.php"; ?>
<link rel="stylesheet" type="text/css" href="css/style.css">
</head>
<body>
<div id="container">
	<div id="output">
	</div>
</div>
<div class="box">
	<div class="wechat">
		
		<div class="sidestrip">
			<div class="am-dropdown" data-am-dropdown>
				<!--头像插件-->
				<div class="own_head am-dropdown-toggle"></div>
				<div class="am-dropdown-content">
					<div class="own_head_top">
						<div class="own_head_top_text">
							<p class="own_name"><?php echo $nickname; ?><img src="images/icon/head.png" alt="" /></p>
							<p class="own_numb"><?php echo $userid; ?></p>
						</div>
						<!-- <img src=<?php echo "'".$photourl."'"; ?> alt="" /> -->
						<img <?php echo 'src="'.$photourl.'"'; ?> alt="" />
						<!-- src="/var/www/zhangjunshuo_netcode/usersPhoto/default.jpg" -->
						<!-- <img src="./images/head/default.jpg" alt="" /> -->
					</div>
					<div class="own_head_bottom">
						<p><span>个性签名</span><?php echo $selfintro; ?></p>
						<div class="own_head_bottom_img">
							<a href=""><img src="images/icon/head_1.png"/></a>
							<a href=""><img src="images/icon/head_2.png"/></a>
						</div>
					</div>
				</div>
			</div>
			<!--三图标-->
			<div class="sidestrip_icon">
				<a id="si_1" href="./chatPage.php"  style="background: url(images/icon/head_2_1.png) no-repeat;"></a>
				<a id="si_2" href="./contactPage.php"></a>
				<a id="si_3" href="./selfPage.php"></a>
			</div>
			
			<!--底部扩展键-->
			<div id="doc-dropdown-justify-js">
				<div class="am-dropdown" id="doc-dropdown-js" style="position: initial;">
					<div class="sidestrip_bc am-dropdown-toggle"></div>
					<ul class="am-dropdown-content" style="">
						<li>
							<a href="#" data-am-modal="{target: '#doc-modal-1', closeViaDimmer: 0, width: 400, height: 225}">意见反馈</a>
							<div class="am-modal am-modal-no-btn" tabindex="-1" id="doc-modal-1">
								<div class="am-modal-dialog">
								<div class="am-modal-hd">Modal 标题
									<a href="javascript: void(0)" class="am-close am-close-spin" data-am-modal-close>&times;</a>
								</div>
								<div class="am-modal-bd">
									Modal 内容。本 Modal 无法通过遮罩层关闭。
								</div>
								</div>
							</div>
						</li>
						
						<li><a href="#">备份与恢复</a></li>
						<li><a href="#">设置</a></li>
					</ul>
				</div>	
			</div>	
		</div>
		
		<!--聊天列表-->
		<div class="middle on">
			<div class="wx_search">
				<input type="text" placeholder="搜索"/>
				<button>+</button>
			</div>
			<div class="office_text">
				<ul class="user_list">
					<!-- chat样板 -->
					<!-- <li class="user_active">
						<div class="user_head"><img src="images/head/15.jpg"/></div>
						<div class="user_text">
							<p class="user_name">早安无恙</p>
							<p class="user_message">我是傻逼！，金少凯牛逼！</p>
						</div>
						<div class="user_time">下午 2：54</div>
					</li> -->
					<!-- php -->
					<?php
						for ($i=0;$i<$num;$i++)
						{
								$row = mysqli_fetch_assoc($chatlist);
								echo '<li>';
								echo '<div class="user_head">';
								// 需要朋友头像了
								// 得现查啊！我现在光有friendID
								$sql2 = "SELECT * FROM users WHERE userID = ".$row['friendID'];
								$query_friendhead = mysqli_query($conn, $sql2);
								$friendhead = mysqli_fetch_assoc($query_friendhead);
							//查完了，friendhead['photoURL']，就是URL
								echo '<img src="'.$friendhead['photoURL'].'"/></div>';
								echo '<div class="user_text">';
								echo '<p class="user_name" id="nick_name">'.$friendhead['nickName'].'</p>';
								//又需要最近一条消息了，还得再查
								//最近的一条消息，就是时间最大的那一条
								$sql3 = "SELECT * FROM messages WHERE (userID=".$userid." and friendID=".$friendhead['userID'].") or (userID=".$friendhead['userID']." and friendID=".$userid.") order by time desc";
								$result3 = mysqli_query($conn, $sql3);
								$theFriendAndMyMessages = mysqli_fetch_assoc($result3);
								// 虽然全查出来了，但只拿取了第一行，也就是最近的一条消息

								// echo '<p class="user_message">'.$theFriendAndMyMessages['time'].'<br>'.$theFriendAndMyMessages['data'].'</p>';
								echo '<p class="user_message">'.$theFriendAndMyMessages['data'].'</p>';
								echo '</div>';
								echo '<div class="user_time">'.substr($theFriendAndMyMessages['time'], 2,8).'</div>';
								//css垃圾，字符串长度超过8就完蛋
								echo '</li>';
						}
					?>
				</ul>
			</div>	
		</div>
		
		<!--好友列表-->
		<div class="middle">
			
		</div>
		
		<!--程序列表-->
		<div class="middle">
			
		</div>
	
		<!--聊天窗口-->
		<div class="talk_window">
			<div class="windows_top">
				<div class="windows_top_box">
					<span>早安无恙</span>
					<ul class="window_icon">
						<li><a href=""><img src="images/icon/icon7.png"/></a></li>
						<li><a href=""><img src="images/icon/icon8.png"/></a></li>
						<li><a href=""><img src="images/icon/icon9.png"/></a></li>
						<li><a href=""><img src="images/icon/icon10.png"/></a></li>
					</ul>
					<div class="extend" class="am-btn am-btn-success" data-am-offcanvas="{target: '#doc-oc-demo3'}"></div>
					<!-- 侧边栏内容 -->
					<div id="doc-oc-demo3" class="am-offcanvas">
						<div class="am-offcanvas-bar am-offcanvas-bar-flip">
							<div class="am-offcanvas-content">
								<p><a href="http://music.163.com/#/song?id=385554" target="_blank">网易音乐</a></p>
							</div>
						</div>
					</div>
				</div>
			</div>
			
			<!--聊天内容-->
			<div class="windows_body">
				<div class="office_text" style="height: 100%;">
					<ul class="content" id="chatbox">
						<li class="me"><img src="images/own_head.jpg" title="金少凯"><span>疾风知劲草，板荡识诚臣</span></li>
						<li class="other"><img src="images/head/15.jpg" title="张文超"><span>勇夫安知义，智者必怀仁</span></li>
					</ul>
				</div>
			</div>
			
			<div class="windows_input" id="talkbox">
				<div class="input_icon">
					<a href="javascript:;"></a>
					<a href="javascript:;"></a>
					<a href="javascript:;"></a>
					<a href="javascript:;"></a>
					<a href="javascript:;"></a>
					<a href="javascript:;"></a>
				</div>
				<div class="input_box">
					<textarea name="" rows="" cols="" id="input_box"></textarea>
					<button id="send">发送（S）</button>
				</div>
			</div>
		</div>
	</div>
</div>


<script type="text/javascript" src="js/jquery.min.js"></script>
<script type="text/javascript" src="js/amazeui.min.js"></script>
<script type="text/javascript" src="js/zUI.js"></script>
<script type="text/javascript" src="js/wechat.js"></script>
<script type="text/javascript" src="js/vector.js"></script>

<script type="text/javascript">
//三图标
window.onload=function(){
	function a(){
		var si1 = document.getElementById('si_1');
		var si2 = document.getElementById('si_2');
		var si3 = document.getElementById('si_3');
		si1.onclick=function(){
			si1.style.background="url(images/icon/head_2_1.png) no-repeat"
			si2.style.background="";
			si3.style.background="";
		};
		si2.onclick=function(){
			si2.style.background="url(images/icon/head_3_1.png) no-repeat"
			si1.style.background="";
			si3.style.background="";
		};
		si3.onclick=function(){
			si3.style.background="url(images/icon/head_4_1.png) no-repeat"
			si1.style.background="";
			si2.style.background="";
		};
	};
	function b(){
		var text = document.getElementById('input_box');
		var chat = document.getElementById('chatbox');
		var btn = document.getElementById('send');
		var talk = document.getElementById('talkbox');
		btn.onclick=function()
		{
			if(text.value =='')
			{
				alert('不能发送空消息');
			}
			else
			{
				chat.innerHTML += '<li class="me"><img src="'+'images/own_head.jpg'+'"><span>'+text.value+'</span></li>';
				text.value = '';
				chat.scrollTop=chat.scrollHeight;
				talk.style.background="#fff";
				text.style.background="#fff";
			};
		};
	};
	function clickOneChat()
	{
		var nick_name = document.getElementById('nick_name');
		alert(nick_name.innerHTML);
	}
	a();
	b();
	clickOneChat();
};
</script>
<script type="text/javascript">
		$(function(){
				Victor("container", "output");   //登陆背景函数
		});
</script>


</body>
</html>
