type universe = {
  address : BsWeb3.Eth.address,
  contract : Universe.t
}

type t = {
  web3 : BsWeb3.Web3.t,
  account : BsWeb3.Eth.address,
  universe: universe,
  address_uri: (BsWeb3.Eth.address => string)
}
