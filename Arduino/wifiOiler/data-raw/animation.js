function Animation(){
	this.rad=55;this.speichen=16;
	this.max=360/this.speichen;
	this.oneAngle=Math.PI*2/360;
	this.speed=5;this.step=0;
	this.continue=false;
	this.bCan=this.tCan=this.bCon=this.tCon=null;

	this.paint=function(){
		this.tCon.clearRect(0,0,this.tCan.width,this.tCan.height);
		var angle;
		this.tCon.beginPath();
		for (var i=1; i < (this.speichen+1); i++){
			angle=((i-1)*(Math.PI*2)/this.speichen)+this.oneAngle*this.step;
			var x1=(this.tCan.width/2)+Math.cos(angle)*(this.rad-12);
			var y1=(this.tCan.height/2)+Math.sin(angle)*(this.rad-12);
			angle=(i*(Math.PI*2)/this.speichen)+this.oneAngle*this.step;
			var x2=(this.tCan.width/2)+Math.cos(angle)*5;
			var y2=(this.tCan.height/2)+Math.sin(angle)*5;
			this.tCon.moveTo(x1,y1);this.tCon.lineTo(x2,y2);
			this.tCon.stroke();
		}
		this.step+=this.speed;
		if (this.step >= this.max) this.step=0;
		if (this.continue) requestAnimationFrame(()=>this.paint());
	}
this.arc=function(x,y,r,s,e,c,w){with(this.bCon){beginPath();strokeStyle=c;lineWidth=w;arc(x,y,r,s,e);stroke();}}
this.line=function(x1,y1,x2,y2,c,w){with(this.bCon){beginPath();strokeStyle=c;moveTo(x1,y1);lineTo(x2,y2);lineWidth=w;stroke();}}
this.init=function(p){
	this.bCan=document.createElement("canvas");
	with(this.bCan){id="toplayer";height=150;width=150}
	this.tCan=document.createElement("canvas");
	with(this.tCan){id="bottomlayer";height=150;width=150}
	this.tCan.style.position=this.bCan.style.position="absolute";
	this.tCan.style.top=this.bCan.style.top=0;
	this.tCan.style.left=this.bCan.style.left=0;
	p.appendChild(this.bCan);p.appendChild(this.tCan);
	if(this.bCan.getContext){
		this.bCon=this.bCan.getContext('2d');
		var x=this.bCan.width/2;
		var y=this.bCan.height/2;
		this.arc(x,y, 20,0,2*Math.PI,"#555555",5);
		this.line(x,y,x+50,y-1,"#333333",25);
		this.arc(x,y,this.rad-11,0,2*Math.PI,"#AAAAAA",5);
		this.arc(x,y,this.rad,0,2*Math.PI,"#333333",19);
		this.arc(x,y,7,0,2*Math.PI,"#000000",12);
		this.line(x+4,y-20,2*x, y-8,"#555555",4);
		this.line(x+4,y+20,2*x,y+20,"#555555",4);
	}
	if(this.tCan.getContext){
		this.tCon=this.tCan.getContext('2d');
	  this.tCon.lineWidth=2;
	  this.tCon.strokeStyle="#123456";
		this.paint();
	}
}
this.run=function(){if(!this.continue){this.continue=true;requestAnimationFrame(()=>this.paint());}}
this.stop=function(){this.continue=false;}}