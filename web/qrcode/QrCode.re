type document;
[@bs.val "document"] external document : document = "";

type element;
[@bs.send.pipe : document] external getElementById : string => element = "";

type callback = (string => unit);
[@bs.module "qrcode"] external toDataURL : element => string => Js.Promise.t(string) = "";
