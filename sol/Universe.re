type abi;
[@bs.module] external abi : abi = "./UniverseAbi.json";

type t;
[@bs.send] external contract : BsWeb3.Eth.t => abi => BsWeb3.Eth.address => t = "Contract";

[@bs.scope "methods"] [@bs.send] external getBalance : t => BsWeb3.Eth.contract_method = "";

[@bs.scope "methods"] [@bs.send] external createEvent : t => string => BsWeb3.Eth.contract_method = "";
[@bs.scope "methods"] [@bs.send] external myEvents : t => BsWeb3.Eth.contract_method = "";
