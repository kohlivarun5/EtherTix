type t;
[@bs.val] external get : t = "navigator";
[@bs.get] external userAgent : t => string = "";
