var neko=function({el=null,edge=true}){
	var neko=document.querySelector(el);
	neko.style.cssText = "user-select:none;\
	background: #eee;\
	position: fixed;\
    z-index:10000;\
	box-sizing: border-box;\
	box-shadow: 0 0 8px rgba(0,0,0,.1);\
    border-radius: 4px;\
	background-size: 100% 100%;\
	overflow: hidden;";
	var nekoW = neko.offsetWidth;
	var nekoH = neko.offsetHeight;
	var cuntW = 0;
	var cuntH = 0;
	neko.style.left = '20%';
	neko.style.top = '20%';


	/*move的作用是靠边吸附 action的作用是吸附旋转 */
	function move(obj, w, h) {
		if (obj.direction === 'left') {
			obj.style.left = 0 - w + 'px';
		} else if (obj.direction === 'right') {

			obj.style.left = document.body.offsetWidth - nekoW + w + 'px';
		}
		if (obj.direction === 'top') {
			obj.style.top = 0 - h + 'px';
		} else if (obj.direction === 'bottom') {
			obj.style.top = document.body.offsetHeight - nekoH + h + 'px';
		}
	}

	function rate(obj, a) {
		//  console.log(a);
		obj.style.transform = ' rotate(' + a + ')'
	}

	function action(obj) {

		var dir = obj.direction;

		switch (dir) {
			case 'left':
				rate(obj, '90deg');
				break;
			case 'right':
				rate(obj, '-90deg');
				break;
			case 'top':
				rate(obj, '-180deg');
				break;
			default:
				rate(obj, '-0');
				break;
		}
	}
	neko.onmousedown = function (e) {
		var nekoL = e.clientX - neko.offsetLeft;
		var nekoT = e.clientY - neko.offsetTop;
		document.onmousemove = function (e) {
			cuntW = 0;
			cuntH = 0;
			neko.direction = '';
			neko.style.transition = '';
			neko.style.left = (e.clientX - nekoL) + 'px';
			neko.style.top = (e.clientY - nekoT) + 'px';
			if (e.clientX - nekoL < 5) {
				neko.direction = 'left';
			}
			if (e.clientY - nekoT < 5) {
				neko.direction = 'top';
			}
			if (e.clientX - nekoL > document.body.offsetWidth - nekoW - 5) {
				neko.direction = 'right';
			}
			if (e.clientY - nekoT > document.body.offsetHeight - nekoH - 5) {
				neko.direction = 'bottom';
			}

			//move(neko, 0, 0);
		}
	}
	// neko.onmouseover = function () {
	// 	move(this, 0, 0);
	// 	rate(this, 0)
	// }

	neko.onmouseout = function () {
		//move(this, nekoW / 2, nekoH / 2);
		//action(this);
	}

	neko.onmouseup = function () {
		document.onmousemove = null;
		this.style.transition = '.2s';
		//move(this, nekoW / 2, nekoH / 2);
		//action(this);
	}

	window.onresize = function () {
		var bodyH = document.body.offsetHeight;
		var nekoT = neko.offsetTop;
		var bodyW = document.body.offsetWidth;
		var nekoL = neko.offsetLeft;

		if (nekoT + nekoH > bodyH) {
			neko.style.top = bodyH - nekoH + 'px';
			cuntH++;
		}
		if (bodyH > nekoT && cuntH > 0) {
			neko.style.top = bodyH - nekoH + 'px';
		}
		if (nekoL + nekoW > bodyW) {
			neko.style.left = bodyW - nekoW + 'px';
			cuntW++;
		}
		if (bodyW > nekoL && cuntW > 0) {
			neko.style.left = bodyW - nekoW + 'px';
		}

		//1move(neko, nekoW / 2, nekoH / 2);
	}
}