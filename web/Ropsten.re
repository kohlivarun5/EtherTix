let address_uri(address) = 
  Js.String.concat(address,"https://ropsten.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0xb3ee8c5d8C4789736c887E5CcFf4Cd2702a0DA7E",
  address_uri
}

