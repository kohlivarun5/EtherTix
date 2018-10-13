let address_uri(address) = 
  Js.String.concat(address,"https://rinkeby.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0xbf054dd712a8bc6ed14820e671277cb6857c6b45",
  address_uri
}
