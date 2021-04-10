type abi
external abi : abi = "EventAbi.json" [@@bs.module]

type t
external contract : BsWeb3.Eth.t -> abi -> BsWeb3.Eth.address -> t = "Contract" [@@bs.send]

module ERC721 : sig 
  external balanceOf : t -> BsWeb3.Eth.address -> int BsWeb3.Eth.contract_method= "" [@@bs.scope "methods"] [@@bs.send]
end

module ERC721MetaData : sig
  external name : t -> string BsWeb3.Eth.contract_method = "" [@@bs.scope "methods"] [@@bs.send]
end

module ERC721Enumerable : sig
  external totalSupply : t -> int BsWeb3.Eth.contract_method= "" [@@bs.scope "methods"] [@@bs.send]
  external tokenOfOwnerByIndex : t -> BsWeb3.Eth.address -> int -> int BsWeb3.Eth.contract_method= "" [@@bs.scope "methods"] [@@bs.send]
end

module Info : sig
  external imgSrc : t -> string BsWeb3.Eth.contract_method = "" [@@bs.scope "methods"] [@@bs.send]
  external numSoldUsed : t -> (int * int) BsWeb3.Eth.contract_method = "" [@@bs.scope "methods"] [@@bs.send]
  external ticketInfo : t -> int -> (bool * BsWeb3.Types.big_number * BsWeb3.Eth.address * bool * BsWeb3.Types.big_number) BsWeb3.Eth.contract_method = "" [@@bs.scope "methods"] [@@bs.send]
  external getCostFor : t -> int -> BsWeb3.Types.big_number BsWeb3.Eth.contract_method = "" [@@bs.scope "methods"] [@@bs.send]
end

module Edit : sig
  external withdraw : t -> unit BsWeb3.Eth.contract_method = "" [@@bs.scope "methods"] [@@bs.send]
  external issue : t -> number:int -> price:BsWeb3.Types.big_number -> unit BsWeb3.Eth.contract_method = "" [@@bs.scope "methods"] [@@bs.send]
  external buy : t -> int -> unit BsWeb3.Eth.contract_method = "" [@@bs.scope "methods"] [@@bs.send]
  external setImg : t -> string -> unit BsWeb3.Eth.contract_method = "" [@@bs.scope "methods"] [@@bs.send]
end

module Use : sig
  external verificationCode : t -> int -> string BsWeb3.Eth.contract_method = "ticketVerificationCode" [@@bs.scope "methods"] [@@bs.send]
  external isOwnerSig : t -> int -> string -> bool BsWeb3.Eth.contract_method = "" [@@bs.scope "methods"] [@@bs.send]
  external useTicket : t -> int -> string -> bool BsWeb3.Eth.contract_method = "" [@@bs.scope "methods"] [@@bs.send]
end

module Market : sig
  external proposeSales : t -> int Js.Array.t -> BsWeb3.Types.big_number Js.Array.t -> unit BsWeb3.Eth.contract_method = "" [@@bs.scope "methods"] [@@bs.send]
  external retractSales : t -> int Js.Array.t -> unit BsWeb3.Eth.contract_method = "" [@@bs.scope "methods"] [@@bs.send]
  external hitAsk : t -> int -> unit BsWeb3.Eth.contract_method = "" [@@bs.scope "methods"] [@@bs.send]

  type filter_options  = {
    fromBlock : int;
    toBlock : string;
  } [@@bs.deriving abstract]

  type ask_event_data
  external ask_event_token: ask_event_data -> int = "token" [@@bs.scope "returnValues"] [@@bs.get]
  external ask_event_ask: ask_event_data -> BsWeb3.Types.big_number  = "ask" [@@bs.scope "returnValues"] [@@bs.get]
  external askEvents : t -> (_ [@bs.as "EventAsk"]) -> filter_options -> ask_event_data Js.Array.t Js.Promise.t = "getPastEvents" [@@bs.send]

end