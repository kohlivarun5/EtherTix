let address_uri(address) =
  Js.String.concat(address,"https://etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0xa87D9e0fC7A9fFaa9aa0A5077df97a6b8aE0cC2D",
  address_uri
}
