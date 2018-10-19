let address_uri(address) = 
  Js.String.concat(address,"https://rinkeby.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0x13db3e4f2f1fd53863cbff499e72d234f0cfe58c",
  address_uri
}
