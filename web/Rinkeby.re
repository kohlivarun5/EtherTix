let address_uri(address) = 
  Js.String.concat(address,"https://rinkeby.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0xCE8464AB3c31B276712129f9c3B65cB93bA6F21C",
  address_uri
}
