let address_uri(address) =
  Js.String.concat(address,"https://etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0x47d67c335d6Cf3103Bc7ea75f5B4041cC6707692",
  address_uri
}
