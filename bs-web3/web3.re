type global;
[@bs.val "web3"] external get : Js.undefined(global) = "";

type provider;
[@bs.get] external currentProvider : global => provider = "";

type address = string;
[@bs.get] external getEnable: provider => Js.undefined(((.unit) => Js.Promise.t(Js.Undefined.t(Js.Array.t(address))))) = "enable";
[@bs.send] external enable : provider => Js.Promise.t(unit) = "";

type t;
[@bs.new] [@bs.module] external makeWeb3 : provider => t = "web3";
[@bs.get] external getCurrentProvider : t => provider = "currentProvider";

[@bs.scope "currentProvider"] [@bs.send] external scanQRCode : t => Js.Re.t => Js.Promise.t(string) = "scanQRCode";
type scanner = (Js.Re.t => Js.Promise.t(string));
[@bs.scope "currentProvider"] [@bs.get] external getScanQrCode : t => Js.undefined(scanner) = "scanQRCode";

[@bs.get] external eth : t => Eth.t = "";

type version;
[@bs.get] external version : global => version = "";

type msg_params;
[@bs.obj] external msg_params : 
                      (~name:string) => 
                      (~type__:string) =>
                      (~value:'a) => 
                      msg_params = "";

type send_async_params;
[@bs.obj] external send_async_params : 
                    (~method__:string) => 
                    (~params:(Js.Array.t(msg_params),Eth.address)) =>
                    (~from:Eth.address) => 
                    send_async_params = "";

type async_result;
type send_async_callback = string => async_result => unit;
[@bs.send.pipe : provider] external sendAsync : send_async_params => send_async_callback => unit = "";

[@bs.get] external async_result_sha : async_result => string = "result";
