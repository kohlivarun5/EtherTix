type abi;
[@bs.module] external abi : abi = "UniverseAbi.json";

type t;
[@bs.send] external contract : BsWeb3.Eth.t => abi => BsWeb3.Eth.address => t = "Contract";
[@bs.scope "methods"] [@bs.send] external isOwner : t => BsWeb3.Eth.contract_method = "";
[@bs.scope "methods"] [@bs.send] external getBalance : t => BsWeb3.Eth.contract_method = "";
[@bs.scope "methods"] [@bs.send] external withdraw : t => BsWeb3.Eth.contract_method = "";

[@bs.scope "methods"] [@bs.send] external createEvent : t => string => BsWeb3.Eth.contract_method = "";

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

type organizer_events_watcher('a) = 'a => organizerEventsData => unit;
[@bs.scope "events"] [@bs.send] external organizerEvents : 
  t => 
  filter_options(organizerEventsQuery) => 
  organizer_events_watcher('a) =>
  unit = "OrganizerEvents";


[@bs.deriving abstract]
type userEventsQuery = {
  [@bs.optional] eventAddr: BsWeb3.Eth.address,
  [@bs.optional] userAddr : BsWeb3.Eth.address,
  [@bs.optional] active : bool
};

type userEventsData;
[@bs.scope "returnValues"] [@bs.get] external userEventAddr : userEventsData => BsWeb3.Eth.address = "eventAddr";
[@bs.scope "returnValues"] [@bs.get] external userAddr : userEventsData => BsWeb3.Eth.address = "";
type user_events_watcher('a) = 'a => userEventsData => unit;
[@bs.scope "events"] [@bs.send] external userEvents : 
  t => 
  filter_options(userEventsQuery) => 
  user_events_watcher('a) => 
  unit = "UserEvents";
