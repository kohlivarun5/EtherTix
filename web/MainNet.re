let address_uri(address) = 
  Js.String.concat(address,"https://etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0x91af175C3CBDEabBd3C307653660F9C5ab044907",
  address_uri
}
