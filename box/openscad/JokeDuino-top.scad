// JokeDuino top 

$wt 	= 1;  // wall thickness
$width 	= 88; // inner width
$height = 58; // inner width
$depth	= 20; // inner depth
$buttons = 8;  // button hole diameter 



module buttons(wt, depth ) {
	for( y = [ 0, 15]) {
		translate([15  , y, -depth/2 - wt/2]){
			cylinder(h=wt*2, d=7,center=true);
		}
	}
}

module topBox(wt,width, height, depth) {
	difference() {
		translate([0,0,-wt/2]) {
			cube([width + wt*2, height+wt*2 , depth+wt], center=true);
		}		
		cube([width, height, depth], center=true);
   	}
}

module topBoxWithButtons(wt, width, height, depth){
	difference(){
		topBox(wt, width, height, depth);
		buttons(wt, depth);
	}
}

module edge(wt, width, height, depth){
		translate([0,0, depth/2 -1]) {
			difference() {
				cube([width,height, 5 ],center=true);
				cube([width-wt, height - 2*wt, 5],center=true);
			}
		}
}

module topBoxWithButtonsAndEdge(wt, width, height, depth){
	union(){
		topBoxWithButtons(wt, width, height, depth);
		edge(wt, width, height, depth);
	}
}

module sensor(width, height, depth){
		translate([width/2 -10, -height/2, depth/2]){
			rotate([-90,0,0]) {
				cylinder(d=6,h=10,center=true);
			}
		}
}




module boxWithSensor(wt, width, height, depth){
	difference(){
		topBoxWithButtonsAndEdge(wt, width, height, depth);
		sensor(width, height, depth);
	}
}

module label(wt, height){
	linear_extrude(height = wt, center = true, convexity = 10, twist = 0, slices = 20, scale = 1.0) {
		text("JokeDuino");
	}
}	



module labelFront(wt, height){
	translate([-30, height/2 + wt+0.2 ,4]){
		rotate([90,180,180]){
			label(wt, height);
		}
	}	
}				

module labelBack(wt, height){
	translate([30, - (height/2 + wt+0.2) ,4]){
		rotate([90,180,0]){
			label(wt,height);
		}
	}	
}

union(){
	boxWithSensor($wt, $width, $height, $depth);
	labelFront($wt, $height);
	labelBack($wt, $height);
}


