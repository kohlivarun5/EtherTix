let address_uri(address) =
  Js.String.concat(address,"https://etherscan.io/address/")

let t : NetworkInfo.t = {
  universe:"0xb803ea85F1B56e5C6442fb931496Bb261158118a",
  address_uri
}
