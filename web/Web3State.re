type t = {
  web3 : BsWeb3.Web3.t,
  account : BsWeb3.Eth.address,
  universe: Universe.t,
  address_uri: (BsWeb3.Eth.address => string)
}
