type abi;
[@bs.module] external abi : abi = "./EventAbi.json";

type t;
[@bs.send] external contract : BsWeb3.Eth.t => abi => BsWeb3.Eth.address => t = "Contract";

[@bs.scope "methods"] [@bs.send] external description : t => BsWeb3.Eth.contract_method = "";

type data = {
  address:BsWeb3.Eth.address,
  description : string
};
