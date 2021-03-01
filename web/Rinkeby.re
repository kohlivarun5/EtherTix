let address_uri(address) = 
  Js.String.concat(address,"https://rinkeby.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0xE92dd037aAECdb3115bad4583F57E1D0C9bB5640",
  address_uri
}
