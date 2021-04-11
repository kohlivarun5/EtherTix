type abi;
[@bs.module] external abi : abi = "UniverseAbi.json";

type t;
[@bs.send] external contract : BsWeb3.Eth.t => abi => BsWeb3.Eth.address => t = "Contract";
[@bs.scope "methods"] [@bs.send] external isOwner : t => BsWeb3.Eth.contract_method(bool) = "";
[@bs.scope "methods"] [@bs.send] external getBalance : t => BsWeb3.Eth.contract_method(BsWeb3.Types.big_number) = "";
[@bs.scope "methods"] [@bs.send] external withdraw : t => BsWeb3.Eth.contract_method(unit) = "";

[@bs.scope "methods"] [@bs.send] external createEvent : t => (~description:string) => (~imgSrc:string) => BsWeb3.Eth.contract_method(unit) = "";

[@bs.deriving abstract]
type organizerEventsQuery = {
  [@bs.optional] eventAddr: BsWeb3.Eth.address,
  [@bs.optional] organizerAddr : BsWeb3.Eth.address,
  [@bs.optional] active : bool
};

type organizerEventsData; 
[@bs.scope "returnValues"] [@bs.get] external organizerEventAddr : organizerEventsData => BsWeb3.Eth.address = "eventAddr";
[@bs.scope "returnValues"] [@bs.get] external organizerEventDesc : organizerEventsData => BsWeb3.Eth.address = "description";

[@bs.deriving abstract]
type filter_options('a) = {
  [@bs.optional] filter : 'a,
  fromBlock : int,
  toBlock : string ,
};

[@bs.send] external organizerEvents : 
  t => 
  ([@bs.as "OrganizerEvents"] _ ) => 
  filter_options(organizerEventsQuery) => 
  Js.Promise.t(Js.Array.t(organizerEventsData)) = "getPastEvents";


[@bs.deriving abstract]
type userEventsQuery = {
  [@bs.optional] eventAddr: BsWeb3.Eth.address,
  [@bs.optional] userAddr : BsWeb3.Eth.address,
  [@bs.optional] active : bool
};

type userEventsData;
[@bs.scope "returnValues"] [@bs.get] external userEventAddr : userEventsData => BsWeb3.Eth.address = "eventAddr";

[@bs.send] external userEvents : 
  t => 
  ([@bs.as "UserEvents"] _ ) => 
  filter_options(userEventsQuery) => 
  Js.Promise.t(Js.Array.t(userEventsData)) = "getPastEvents";
