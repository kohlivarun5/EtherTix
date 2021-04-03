let address_uri(address) = 
  Js.String.concat(address,"https://rinkeby.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0xBdf740190dBE6216863d1F7627F012DAdF952F8d",
  address_uri
}
