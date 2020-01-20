let address_uri(address) =
  Js.String.concat(address,"https://etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0x106E87B62f5172a62D020B9922B5abb09f856294",
  address_uri
}
