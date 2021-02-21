[@bs.deriving abstract]
type wallet_connect_options = {
  infuraId:string
};

type wallet_connect;
[@bs.module "@walletconnect/web3-provider"] external wallet_connect : wallet_connect = "default";

[@bs.deriving abstract]
type provider_options = {
    package:wallet_connect,
    options:wallet_connect_options
};

type t;
[@bs.new] [@bs.module "web3modal"] external web3_provider: provider_options => t = "default";
[@bs.send] external connect : t => Js.Promise.t(BsWeb3.Web3.provider) = "";