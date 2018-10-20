let address_uri(address) = 
  Js.String.concat(address,"https://etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0x1145834aE3843D5f36A32E1bCae1f3C19BC82788",
  address_uri
}
