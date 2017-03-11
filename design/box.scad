shell_t = 2;
inner_w = 50;
inner_h = 35;
inner_l = 15;
corner_r = 2;

lcd_opening_w = 26;
lcd_opening_h = 14;
lcd_up_offset = 6.5;
lcd_opening_x_margin = 5;

lcd_pegs_span_x = 23.5;
lcd_pegs_span_y = 24;
lcd_pegs_d = 1.5;
lcd_pegs_h = 4 + shell_t;

button_d = 6.5;
button_x_margin = 5;

snap_slit_width = 6;
snap_slit_height = 2.5;
snap_slit_z = 5;

usb_slit_w = 10;
usb_slit_h = 8;
usb_slit_depth = 8;

cage_inner_wx = 35;
cage_inner_wy = 26.5;
cage_h = 7;
cage_t = 1.5;
cage_claw_wy = 10;
cage_claw_wx = 15;

tolerance = 0.5;

$fs = 0.01;

module box(width, height, length) {
     translate([inner_w / 2, inner_h / 2, 0]) linear_extrude(length) minkowski() {
         circle(r=corner_r);
         square([width - 2*corner_r, height - 2*corner_r], center = true);
     }
}

module box_shell() {
    difference() {
        box(inner_w + 2*shell_t, inner_h + 2*shell_t, inner_l + shell_t);
        translate([0, 0, -shell_t]) box(inner_w, inner_h, inner_l + shell_t);
    }
}

module lcd_pegs() {
    translate([-lcd_pegs_span_x / 2, -lcd_pegs_span_y / 2])
        cylinder(d=lcd_pegs_d, h=lcd_pegs_h);
    translate([lcd_pegs_span_x / 2, -lcd_pegs_span_y / 2])
        cylinder(d=lcd_pegs_d, h=lcd_pegs_h);
    translate([-lcd_pegs_span_x / 2, lcd_pegs_span_y / 2])
        cylinder(d=lcd_pegs_d, h=lcd_pegs_h);
    translate([lcd_pegs_span_x / 2, lcd_pegs_span_y / 2])
        cylinder(d=lcd_pegs_d, h=lcd_pegs_h);
}

module side_slits() {
    translate([inner_w / 2, 0, snap_slit_z])
        cube([snap_slit_width, 2*shell_t, snap_slit_height], center=true);
    translate([inner_w / 2, inner_h, snap_slit_z])
        cube([snap_slit_width, 2*shell_t, snap_slit_height], center=true);
}

module usb_slit() {
    translate([inner_w - usb_slit_depth / 2 + shell_t, inner_h/2, usb_slit_h / 2 - shell_t - 1])
        cube([usb_slit_depth + 1, usb_slit_w, usb_slit_h + 1], center = true);
}

module box_main() {
    difference() {
        box_shell();
        
        // Screen opening
        translate([inner_w - lcd_opening_w/2 - lcd_opening_x_margin,
            inner_h / 2, inner_l + 0.5*shell_t])
        cube([lcd_opening_w, lcd_opening_h, 2*shell_t], center=true);
        
        // Button opening
        translate([button_x_margin, (inner_h - button_d) / 2, inner_l - 0.5*shell_t])
            cube([button_d, button_d, 2*shell_t]);
        
        side_slits();        
        usb_slit();
    }
    
    translate([inner_w - lcd_opening_w/2 - lcd_opening_x_margin, inner_h / 2,
        inner_l - lcd_pegs_h + shell_t]) lcd_pegs();
}

module snap_tab() {
    tab_width = snap_slit_width - tolerance;
    snap_tab_r = snap_slit_height / 2;
    translate([(inner_w - tab_width) / 2, 0, 0]) {
        cube([tab_width, shell_t, snap_slit_z + snap_tab_r]);
        translate([0, 0, snap_slit_z])
            rotate([0, 90, 0]) cylinder(h=tab_width, r=snap_tab_r);
    }    
}

module board_cage() {
    translate([cage_t, 0, 0]) {
        cube([cage_inner_wx, cage_t, cage_h]);
        translate([0, cage_inner_wy + cage_t, 0]) cube([cage_inner_wx, cage_t, cage_h]);
    }
    cube([cage_t, cage_inner_wy + 2*cage_t, cage_h]);
    translate([0, cage_t + (cage_inner_wy - cage_claw_wy) / 2, cage_h - cage_t])
        cube([cage_claw_wx, cage_claw_wy, cage_t]);
}

module bottom_lid() {
    difference() {
        union() {
            translate([0, 0, -shell_t]) box(inner_w + 2*shell_t, inner_h + 2*shell_t, shell_t);
            snap_tab();
            translate([0, inner_h, 0]) mirror([0, 1, 0]) snap_tab();
            
            translate([inner_w - (cage_inner_wx + cage_t),
                (inner_h - (cage_inner_wy + 2*cage_t)) / 2, 0]) board_cage();
        }
        usb_slit();
    }
}

box_main();
//bottom_lid();