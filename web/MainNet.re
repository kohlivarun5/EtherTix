let address_uri(address) = 
  Js.String.concat(address,"https://etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0x3d6a0f5b196b23bfe8597c28906f77305d5fd9bf",
  address_uri
}
