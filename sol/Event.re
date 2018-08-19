type abi;
[@bs.module] external abi : abi = "EventAbi.json";

type t;
[@bs.send] external contract : BsWeb3.Eth.t => abi => BsWeb3.Eth.address => t = "Contract";

[@bs.scope "methods"] [@bs.send] external description : t => BsWeb3.Eth.contract_method = "";
[@bs.scope "methods"] [@bs.send] external getBalance : t => BsWeb3.Eth.contract_method = "";
[@bs.scope "methods"] [@bs.send] external withdraw : t => BsWeb3.Eth.contract_method = "";

[@bs.scope "methods"] [@bs.send] external numSold : t => BsWeb3.Eth.contract_method = "";
[@bs.scope "methods"] [@bs.send] external numUnSold : t => BsWeb3.Eth.contract_method = "";

[@bs.scope "methods"] [@bs.send] external issue : t => (~number:int) => (~price_szabo:int) => BsWeb3.Eth.contract_method = "";

[@bs.scope "methods"] [@bs.send] external getCostFor : t => (~numTickets:int) => BsWeb3.Eth.contract_method = "";
[@bs.scope "methods"] [@bs.send] external buy : t => (~numTickets:int) => BsWeb3.Eth.contract_method = "";



let ofAddress(web3,address) {
  let eth = BsWeb3.Web3.eth(web3);
  Js.log((eth,address,abi));
  let event:t = [%bs.raw{| new eth.Contract(EventAbiJson.default,address) |}];
  event 
}
