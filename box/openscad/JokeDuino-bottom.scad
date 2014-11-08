// JokeDuino top 

$wt 	= 1;  // wall thickness
$width 	= 88; // inner width
$height = 58; // inner width
$depth	= 30; // inner depth
$buttons = 8;  // button hole diameter 

module wall(wt,width, height, depth){
	wallT = 1;
	wallH = 17;
	moveX = - (height - (width/2) + wallT);
	moveZ = - (depth - wallH)/2;
	translate([ moveX,0,moveZ]){
		cube([wallT, height , wallH], center=true);
	}
}

module grille() {
	for( y = [-13, 0, 13]) {
		for (x = [-13, 0, 13]){
			translate([x,y,-15]){
				cylinder(d=6,h=2,center=true);
			}
		}
	}
}

module bottomBox(wt,width, height, depth) {
	difference() {
		translate([0,0,-wt/2]) {
			cube([width + wt*2, height+wt*2 , depth+wt], center=true);
		}		
		cube([width, height, depth], center=true);
   	}
}


module bottomBoxWithGrille(wt,width, height, depth){
	difference(){
		bottomBox(wt,width, height, depth);
		translate([15,0,0]){
			grille();
		}
	}
}

module sensor(width, height, depth){
		translate([width/2 - 8, height/2, depth/2]){
			rotate([-90,0,0]) {
				cylinder(d=6,h=10,center=true);
			}
		}
}

module box(wt, width, height, depth){
	union(){
		difference(){
			bottomBoxWithGrille(wt, width, height, depth);
			sensor(width, height, depth);
		}
		wall(wt, width, height, depth);
	}
}

module label(wt, height){
	linear_extrude(height = wt, center = true, convexity = 10, twist = 0, slices = 20, scale = 1.0) {
		text("JokeDuino");
	}		
}	

module labelFront(wt, height){
	translate([30, height/2 + wt+0.2 ,-4]){
		rotate([90,0,180]){
			label(wt,height);
		}
	}
}

module labelBack(wt, height){
	translate([-30, - (height/2 + wt +0.2 ) ,-4]){
		rotate([90,0,0]){
			label(wt,height);
		}
	}	
}

union(){
	box($wt,$width, $height, $depth);
	labelFront($wt,$height);
	labelBack($wt,$height);
}

