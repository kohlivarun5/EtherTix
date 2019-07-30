let address_uri(address) = 
  Js.String.concat(address,"https://etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0x93e5614e9BC32C8bBbCcAFEe6fDba0610E60a31A",
  address_uri
}
