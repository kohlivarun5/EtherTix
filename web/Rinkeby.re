let address_uri(address) = 
  Js.String.concat(address,"https://rinkeby.etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0xee713Fa879857701A176c55Fa983070Cb0A2a723",
  address_uri
}
