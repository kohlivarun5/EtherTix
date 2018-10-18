type abi;
[@bs.module] external abi : abi = "EventAbi.json";

type t;
[@bs.send] external contract : BsWeb3.Eth.t => abi => BsWeb3.Eth.address => t = "Contract";

[@bs.scope "methods"] [@bs.send] external description : t => BsWeb3.Eth.contract_method = "";
[@bs.scope "methods"] [@bs.send] external getBalance : t => BsWeb3.Eth.contract_method = "";
[@bs.scope "methods"] [@bs.send] external withdraw : t => BsWeb3.Eth.contract_method = "";

[@bs.scope "methods"] [@bs.send] external numSold : t => BsWeb3.Eth.contract_method = "";
[@bs.scope "methods"] [@bs.send] external numUnSold : t => BsWeb3.Eth.contract_method = "";

[@bs.scope "methods"] [@bs.send] external issue : t => (~number:int) => (~price:BsWeb3.Types.big_number) => BsWeb3.Eth.contract_method = "";

[@bs.scope "methods"] [@bs.send] external getCostFor : t => (~numTickets:int) => BsWeb3.Eth.contract_method = "";
[@bs.scope "methods"] [@bs.send] external buy : t => (~numTickets:int) => BsWeb3.Eth.contract_method = "";
[@bs.scope "methods"] [@bs.send] external myTickets : t => BsWeb3.Eth.contract_method = "";

[@bs.scope "methods"] [@bs.send] external ticketVerificationCode : t => int => BsWeb3.Eth.contract_method = "";
[@bs.scope "methods"] [@bs.send] external isOwnerSig : t => int => string => BsWeb3.Eth.contract_method = "";

[@bs.scope "methods"] [@bs.send] external numUsed : t => BsWeb3.Eth.contract_method = "";
[@bs.scope "methods"] [@bs.send] external numToBeUsed : t => BsWeb3.Eth.contract_method = "";

[@bs.scope "methods"] [@bs.send] external ticketUsed : t => int => BsWeb3.Eth.contract_method = "";
[@bs.scope "methods"] [@bs.send] external useTicket : t => int => string => BsWeb3.Eth.contract_method = "";

[@bs.scope "methods"] [@bs.send] external proposeSale : t => Js.Array.t(int) => (~price:BsWeb3.Types.big_number) => BsWeb3.Eth.contract_method = "";
[@bs.scope "methods"] [@bs.send] external retractSale : t => Js.Array.t(int) => BsWeb3.Eth.contract_method = "";
[@bs.scope "methods"] [@bs.send] external forSale : t => BsWeb3.Eth.contract_method = "";
[@bs.scope "methods"] [@bs.send] external hitAsk : t => int => BsWeb3.Eth.contract_method = "";


let ofAddress(web3,address) {
  let eth = BsWeb3.Web3.eth(web3);
  Js.log((eth,address,abi));
  let event:t = [%bs.raw{| new eth.Contract(EventAbiJson.default,address) |}];
  event 
}
